/*
Datenstruktur

$Copyright (C) 2000 Erich Fr�hst�ck
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
#include <EFEU/strbuf.h>
#include <EFEU/efio.h>

void SrcData_init (SrcData *data, IO *ein)
{
	memset(data, 0, sizeof(SrcData));
	DocBuf_init(&data->doc);
	sb_init(&data->buf, 0);
	data->ein = ein;
}


void SrcData_write (SrcData *data, IO *io)
{
	DocBuf_write(&data->doc, io);
	sb_free(&data->buf);
}
