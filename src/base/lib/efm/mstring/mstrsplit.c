/*
Aufspalten eines Strings

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <EFEU/strbuf.h>

#define	SPACE(x)	((x) == ' ' || (x) == '\t')

char *nextstr (char **ptr)
{
	char *p = *ptr;

	if	(p)
	{
		while (**ptr != 0)
			(*ptr)++;

		(*ptr)++;
		return (*p == 0 ? NULL : p);
	}

	return NULL;
}

size_t mstrsplit(const char *str, const char *delim, char ***ptr)
{
	StrBuf *sb;
	size_t i, dim;
	char *p;

	if	(ptr == NULL)	return 0;
	else			*ptr = NULL;

	if	(str == NULL)	return 0;

	sb = sb_create(0);
	i = dim = 0;

/*	String umkopieren
*/
	for (; listcmp(delim, *str); str++)
	{
		if	(!SPACE(*str))
		{
			sb_putc(0, sb);
			dim++;
		}
	}

	while (*str != 0)
	{
		if	(listcmp(delim, *str))
		{
			for (; listcmp(delim, *str); str++)
			{
				if	(!SPACE(*str))
				{
					sb_putc(0, sb);
					dim++;
					i = 0;
				}
			}

			if	(i)
			{
				sb_putc(0, sb);
				dim++;
				i = 0;
			}
		}
		else
		{
			sb_putc(*str, sb);
			str++;
			i++;
		}
	}

	if	(i)
	{
		sb_putc(0, sb);
		dim++;
	}

/*	Initialisieren der Teilstringstruktur
*/
	if	(dim)
	{
		*ptr = (char **) memalloc(sb->pos + dim * sizeof(char *));
		p = (char *) ((*ptr) + dim);
		memcpy(p, sb->data, sb->pos);

		for (i = 0; i < dim; i++, p++)
			for ((*ptr)[i] = p; *p != 0; p++)
				;
	}

	rd_deref(sb);
	return dim;
}
