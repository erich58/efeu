/*
Standardzähler

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

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

static void add (MdCount *cnt, void *data, void *buf)
{
	Val_int(data)++;
}

static MdCount counter = {
	"count",
	"int",
	"standard counter",
	NULL,
	NULL,
	add,
};

/*
Die globale Variable |$1| definiert einen Standardzähler
mit |int| als Datentyp.
*/

MdCount *stdcount = &counter;
