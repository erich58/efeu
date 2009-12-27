/*
Information ausgeben

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
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>
#include <ctype.h>

Info_t *Info_admin (Info_t *tg, const Info_t *src)
{
	if	(tg && tg->type && tg->type->admin)
		return tg->type->admin(tg, src);

	if	(src && src->type && src->type->admin)
		return src->type->admin(NULL, src);

	return (Info_t *) src;
}

void Info_print (io_t *io, const Info_t *data)
{
	if	(io && data && data->type && data->type->pfunc)
		data->type->pfunc(io, data);
}

void Info_subprint (io_t *io, const Info_t *data, const char *pfx)
{
	if	(io && data && data->type && data->type->pfunc)
	{
		io = io_lmark(io_refer(io), pfx, NULL, 0);
		data->type->pfunc(io, data);
		io_close(io);
	}
}
