/*
:*:constant strings
:de:Konstante Zeichenketten

$Header	<EFEU/$1>

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

#include <EFEU/StrData.h>

static void cnst_init (StrData *sd, const char *def)
{
	sd->ptr = (void *) def;
	sd->key = 0;
}

static char *cnst_get (const StrData *sd)
{
	return (char *) sd->ptr + sd->key;
}

static void cnst_copy (StrData *tg, const StrData *src)
{
	*tg = *src;
}

StrCtrl StrCtrl_const = { "const", "constant string",
	cnst_init, NULL, cnst_get, NULL, cnst_copy,
};
