/*
:*:strings with allocated memory
:de:Zeichenketten mit zugewiesenem Speicherbereich

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
#include <EFEU/mstring.h>

static void alloc_init (StrData *sd, const char *str)
{
	sd->ptr = mstrcpy(str);
	sd->key = 0;
}

static char *alloc_get (const StrData *sd)
{
	return (char *) sd->ptr + sd->key;
}

static void alloc_set (StrData *sd, const char *str)
{
	memfree(sd->ptr);
	sd->ptr = mstrcpy(str);
	sd->key = 0;
}

static void alloc_clean (StrData *sd, int destroy)
{
	memfree(sd->ptr);
	sd->ptr = NULL;
	sd->key = 0;
}

StrCtrl StrCtrl_alloc = { "alloc", "allocateted memory",
	alloc_init,
	alloc_clean,
	alloc_get,
	alloc_set,
	NULL,
};
