/*
:*:memmory allocation with overwrite checking
:de:Speicherplatzanforderung mit Überschreibprüfung

$Copyright (C) 1996 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/memalloc.h>

#define	ERR	"sorry: malloc(%lu) failed\n"
#define	ERR2	"sorry: realloc(%p, %lu) failed\n"


#define	CHECK_BSIZE	32
#define	CHECK_SIZE	8
#define	CHECK_MASK	"intactly"

#define	ERR_ALLOC	"sorry: realloc(%p, %zu) failed\n"
#define	ERR_CHECK	"limcheck(%p, %zu): memory overwrite detected.\n"

typedef struct {
	void *data;
	size_t size;
} ENTRY;

static void *realloc_errcheck(void *data, size_t size)
{
	void *ndata = realloc(data, size);

	if	(!ndata)
	{
		fprintf(stderr, ERR_ALLOC, data, size);
		exit(EXIT_FAILURE);
	}

	return ndata;
}

static void check_entry(ENTRY *entry)
{
	void *p = (char *) entry->data + entry->size;

	if	(memcmp(p, CHECK_MASK, CHECK_SIZE) != 0)
	{
		fprintf(stderr, ERR_CHECK, entry->data, entry->size);
		exit(EXIT_FAILURE);
	}
}

static void clear_entry(ENTRY *entry)
{
	memset((char *) entry->data + entry->size, 0, CHECK_SIZE);
}

void *realloc_limcheck (void *odata, size_t size)
{
	static ENTRY *tab = NULL;
	static size_t tabsize = 0;
	static size_t capacity = 0;
	static size_t blksize = CHECK_BSIZE;
	void *ndata  = NULL;

	if	(size)
	{
		ndata = realloc_errcheck(odata, size + CHECK_SIZE);
		memcpy((char *) ndata + size, CHECK_MASK, CHECK_SIZE);
	}

	for (size_t n = 0; n < tabsize; n++)
	{
		check_entry(tab + n);

		if	(tab[n].data == odata)
		{
			clear_entry(tab + n);

			if	(ndata)
			{
				tab[n].data = ndata;
				tab[n].size = size;
				size = 0;
			}
			else
			{
				tabsize--;

				if	(n < tabsize)
				{
					tab[n] = tab[tabsize];
				}
			}
		}
	}

	if	(size)
	{
		if	(tabsize >= capacity)
		{
			capacity += blksize;
			blksize = tabsize ? tabsize : CHECK_BSIZE;
			tab = realloc_errcheck(tab, capacity * sizeof tab[0]);
		}

		tab[tabsize].data = ndata;
		tab[tabsize].size = size;
		tabsize++;
	}

	return ndata;
}
