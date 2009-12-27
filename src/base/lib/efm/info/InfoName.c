/*
Informationskennung zerlegen

$Copyright (C) 1998 Erich Frühstück
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

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <ctype.h>

#define	NAME_SPEC(c)	((c) == INFO_SEP || (c) == INFO_ESC)

char *InfoNameToken (char **pptr)
{
	char *ptr;
	strbuf_t *sb;

	if	(pptr == NULL || *pptr == NULL)
		return NULL;

	sb = new_strbuf(0);

	for (ptr = *pptr; *ptr != 0; ptr++)
	{
		if	(*ptr == INFO_ESC && NAME_SPEC(ptr[1]))
		{
			ptr++;
			sb_putc(*ptr, sb);
		}
		else if	(*ptr == INFO_SEP)
		{
			ptr++;
			break;
		}
		else	sb_putc(*ptr, sb);
	}

	*pptr = *ptr ? ptr : NULL;
	return sb2str(sb);
}

int InfoName(io_t *io, InfoNode_t *base, InfoNode_t *info)
{
	if	(info && info != base)
	{
		if	(InfoName(io, base, info->prev))
			io_putc(INFO_SEP, io);

		if	(info->name)
		{
			register char *p = info->name;

			while (*p != 0)
			{
				if	(NAME_SPEC(*p))
					io_putc(INFO_ESC, io);

				io_putc(*p, io);
				p++;
			}
		}

		return 1;
	}
	else	return 0;
}
