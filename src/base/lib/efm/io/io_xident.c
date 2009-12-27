/*
Identifikationsstring einer IO-Struktur generieren

$Copyright (C) 1997 Erich Frühstück
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
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

char *io_xident (io_t *io, const char *fmt, ...)
{
	strbuf_t *buf;
	char *id, *p;
	va_list list;

	id = io_ident(io);

	if	(fmt == NULL)	return id;

	va_start(list, fmt);
	buf = new_strbuf(0);

	for (; *fmt != 0; fmt++)
	{
		if	(*fmt == '%')
		{
			fmt++;

			switch (*fmt)
			{
			case 'd':
				sb_vprintf(buf, "%d", list);
				break;
			case 's':
				p = va_arg(list, char *);
				sb_puts(p, buf);
				break;
			case 'm':
				p = va_arg(list, char *);
				sb_puts(p, buf);
				memfree(p);
				break;
			case '*':
				sb_puts(id, buf);
				break;
			case 0:
				break;
			default:
				sb_putc(*fmt, buf);
				break;
			}
		}
		else	sb_putc(*fmt, buf);
	}

	va_end(list);
	memfree(id);
	return sb2str(buf);
}
