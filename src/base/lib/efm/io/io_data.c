/*
Ein/Ausgabe auf Datenfeld

$Copyright (C) 2007 Erich Frühstück
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


#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/refdata.h>
#include <EFEU/parsub.h>
#include <EFEU/Debug.h>

#if	REVBYTEORDER
#define	DATA(ptr,size)	(unsigned char *) ptr + size - 1
#define	SET(ptr,val)	*(ptr--) = val
#define	GET(ptr)	*(ptr--)
#else
#define	DATA(ptr,size)	(unsigned char *) ptr
#define	SET(ptr,val)	*(ptr++) = val
#define	GET(ptr)	*(ptr++)
#endif

/*	Stringstruktur
*/

typedef struct {
	void *ref;
	unsigned char *data;
	size_t pos;
	size_t size;
} IODATA;


static IODATA *iodata_alloc (void *ref, void *data, size_t size)
{
	IODATA *s = memalloc(sizeof *s);
	s->ref = rd_refer(ref);
	s->data = data;
	s->size = size;
	s->pos = 0;
	return s;
}

static void iodata_clean (IODATA *s)
{
	rd_deref(s->ref);
	memfree(s);
}


static char *iodata_ident (IODATA *s)
{
	return mpsubarg("$1:$2/$3", "nmUU", rd_ident(s->ref),
		(uint64_t) s->pos, (uint64_t) s->size);
}

static int iodata_get (void *ptr)
{
	IODATA *s = ptr;
	return (s->pos < s->size) ? (int) s->data[s->pos++] : EOF;
}

static size_t iodata_dbread (void *ptr, void *dptr,
	size_t recl, size_t size, size_t n)
{
	IODATA *s = ptr;
	size_t i, j;

	if	(s->pos + n * recl > s->size)
		dbg_error(NULL, "[ftools:21]", NULL);

	for (i = 0; i < n; i++)
	{
		unsigned char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
			SET(data, s->data[s->pos++]);

		for (j = recl; j < size; j++)
			SET(data, 0);

		dptr = (char *) dptr + size;
	}

	return recl * n;
}

static int iodata_ctrl (void *ptr, int req, va_list list)
{
	IODATA *s = ptr;

	switch (req)
	{
	case IO_CLOSE:
		iodata_clean(s);
		return 0;
	case IO_REWIND:
		s->pos = 0;
		return 0;
	case IO_GETPOS:
		*va_arg(list, unsigned *) = s->pos;
		return 0;
	case IO_SETPOS:
		{
			int pos = *va_arg(list, unsigned *);

			if	(pos < s->size)
			{
				s->pos = pos;
				return 0;
			}
			else	return EOF;
		}
	case IO_IDENT:
		*va_arg(list, char **) = iodata_ident(s);
		return 0;
	default:
		return EOF;
	}
}

IO *io_data (void *ref, void *data, size_t size)
{
	if	(data)
	{
		IO *io = io_alloc();
		io->get = iodata_get;
		io->ctrl = iodata_ctrl;
		io->dbread = iodata_dbread;
		io->data = iodata_alloc(ref, data, size);
		return io;
	}

	return NULL;
}
