/*
Temporäre Stringbufferuffer verwalten

$Copyright (C) 2008 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/strbuf.h>

#define	FULL_TRACE	0
#define	PART_TRACE	0

static struct {
	int lock;
	StrBuf buf;
} tmp_tab[] = {
	{ 0, SB_DATA(0) },
	{ 0, SB_DATA(0) },
	{ 0, SB_DATA(0) },
#if	1
	{ 0, SB_DATA(0) },
#endif
};

static int depth = 0;
static int count = 0;
static int flag = 0;

/*
Die Funktion |$1| fordert einen neuen Stringbuffer an.
*/

StrBuf *sb_acquire (void)
{
	StrBuf *buf;
	int i;

	count++;

	for (i = 0; i < tabsize(tmp_tab); i++)
	{
		if	(!tmp_tab[i].lock)
		{
			tmp_tab[i].lock = count;
			sb_trunc(&tmp_tab[i].buf);
#if	FULL_TRACE
			fprintf(stderr, "sb_acquire[%d] %d\n", i, count);
#endif
			return &tmp_tab[i].buf;
		}
	}

	buf = memalloc(sizeof *buf);
	depth++;
	flag = 1;
#if	PART_TRACE
	fprintf(stderr, "sb_acquire[%p] %d %d\n", buf, count, depth);
#endif
	sb_init(buf, 0);
	return buf;
}

/*
$alias
Die Funktion |$1| gibt einen zuvor angeforderten Stringbuffer frei.
*/

void sb_release (StrBuf *buf)
{
	int i;

	if	(!buf)	return;

	for (i = 0; i < tabsize(tmp_tab); i++)
	{
		if	(&tmp_tab[i].buf == buf)
		{
#if	PART_TRACE
#if	FULL_TRACE
#else
			if	(flag || count - tmp_tab[i].lock > 100)
#endif
			{
				fprintf(stderr, "sb_release[%d] %d+%d\n", i,
					tmp_tab[i].lock,
					count - tmp_tab[i].lock);

				if	(i == 0)
					flag = 0;
			}
#endif

			tmp_tab[i].lock = 0;
			return;
		}
	}
	
#if	PART_TRACE
	fprintf(stderr, "sb_release[%p] %d %d\n", buf, count, depth);
#endif
	depth--;
	sb_free(buf);
	memfree(buf);
}

/*
Die Funktion |$1| gibt den Stringbuffer frei und liefert eine Kopie
des Inhalts.
*/

char *sb_cpyrelease (StrBuf *buf)
{
	char *p = sb_strcpy(buf);
	sb_release(buf);
	return p;
}
