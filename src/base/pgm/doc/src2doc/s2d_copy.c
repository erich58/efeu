/*
Kommentarbuffer umkopieren 

$Copyright (C) 2000 Erich Frühstück
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

#include "src2doc.h"
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/efio.h>


void SrcData_copy (SrcData *data, StrBuf *buf, const char *name)
{
	if	(sb_getpos(&data->buf) == 0)	return;

	if	(strncmp("$include", (char *) data->buf.data, 8) == 0)
	{
		char *p, *q;

		sb_putc(0, &data->buf);
		p = (char *) data->buf.data + 8;

		while (*p && (*p == ' ' || *p == '\n'))
			p++;

		if	((q = strchr(p, '\n')))
			*q = 0;

		io_push(data->ein, io_fileopen(p ,"rzd"));
		sb_clean(&data->buf);
	}
	else	DocBuf_copy(&data->doc, &data->buf, buf, name);

	sb_clean(data->doc.source);
}
