/*
Informationsbasis: String

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
#include <EFEU/parsub.h>
#include <ctype.h>

/*	Info-String
*/

static void str_print (io_t *io, const InfoString_t *data)
{
	io_psub(io, data->fmt);
}

static InfoString_t *str_admin (InfoString_t *tg, const InfoString_t *src)
{
	if	(tg)
	{
		memfree(tg->fmt);
		tg->fmt = NULL;

		if	(src == NULL)
		{
			memfree(tg);
			return NULL;
		}

		if	(src == tg)	return tg;
	}
	else
	{
		tg = memalloc(sizeof(InfoString_t));
		tg->type = &InfoType_mstr;

		if	(!src)	return tg;
	}

	tg->fmt = mstrcpy(src->fmt);
	return tg;
}

Info_t *InfoString_create (char *label)
{
	InfoString_t *x = str_admin(NULL, NULL);
	x->fmt = label;
	return (Info_t *) x;
}

InfoType_t InfoType_cstr = { NULL, (InfoPrint_t) str_print };
InfoType_t InfoType_mstr = { (admin_t) str_admin, (InfoPrint_t) str_print };
