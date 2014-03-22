/*
Zeilen aus IO - Struktur in String laden

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

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/efutil.h>

char *io_lgets (IO *io, const char *end)
{
	StrBuf *sb;
	int c;
	int save;
	int len;

	if	(io == NULL)	return NULL;

	sb = sb_acquire();
	save = 0;
	len = end ? strlen(end) : -1;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')
		{
			if	(sb->pos - save == len && strncmp((char *)
					sb->data + save, end, len) == 0)
			{
				sb_setpos(sb, save);
				break;
			}

			save = sb->pos + 1;
			io_linemark(io);
			continue;
		}

		sb_putc(c, sb);
	}

	return sb_cpyrelease(sb);
}
