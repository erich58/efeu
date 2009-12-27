/*
Aufzählungstype aus Label-Tabelle generieren

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

#include <EFEU/object.h>
#include <EFEU/stdtype.h>

EfiType *LabelType (const char *name, Label *label, size_t dim)
{
	EfiType *type;
	int val;
	
	type = NewEnumType(name, EnumTypeRecl(dim));

	for (val = 0; val < dim; val++, label++)
		VarTab_xadd(type->vtab, label->name, label->desc,
			NewObj(type, &val));

	return AddEnumType(type);
}
