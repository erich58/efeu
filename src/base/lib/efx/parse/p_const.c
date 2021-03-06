/*
Konstante Objekte

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
#include <EFEU/parsedef.h>
#include <EFEU/cmdsetup.h>

EfiObj *PFunc_bool (IO *io, void *data)
{
	return bool2Obj(data != NULL);
}

EfiObj *PFunc_int (IO *io, void *data)
{
	return int2Obj((int) (size_t) data);
}

EfiObj *PFunc_str (IO *io, void *data)
{
	return str2Obj(mstrcpy(data));
}

EfiObj *PFunc_type (IO *io, void *data)
{
	return type2Obj(Parse_type(io, data));
}
