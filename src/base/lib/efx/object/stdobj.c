/*
Standardobjekte

$Copyright (C) 2003 Erich Frühstück
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

EfiObj *bool2Obj (int val)
{
	return NewObj(&Type_bool, &val);
}

EfiObj *char2Obj (int val)
{
	unsigned char data = val;
	return NewObj(&Type_char, &data);
}

EfiObj *int2Obj (int val)
{
	return NewObj(&Type_int, &val);
}

EfiObj *uint2Obj (unsigned val)
{
	return NewObj(&Type_uint, &val);
}

EfiObj *varint2Obj (int64_t val)
{
	return NewObj(&Type_varint, &val);
}

EfiObj *varsize2Obj (uint64_t val)
{
	return NewObj(&Type_varsize, &val);
}

EfiObj *float2Obj (double val)
{
	float data = val;
	return NewObj(&Type_float, &data);
}

EfiObj *double2Obj (double val)
{
	return NewObj(&Type_double, &val);
}

int Obj2bool (EfiObj *obj)
{
	int val;
	Obj2Data(obj, &Type_bool, &val);
	return val;
}

int Obj2char (EfiObj *obj)
{
	unsigned char val;
	Obj2Data(obj, &Type_char, &val);
	return val;
}

int Obj2int (EfiObj *obj)
{
	int val;
	Obj2Data(obj, &Type_int, &val);
	return val;
}

unsigned Obj2uint (EfiObj *obj)
{
	unsigned val;
	Obj2Data(obj, &Type_uint, &val);
	return val;
}


float Obj2float (EfiObj *obj)
{
	float val;
	Obj2Data(obj, &Type_float, &val);
	return val;
}

double Obj2double (EfiObj *obj)
{
	double val;
	Obj2Data(obj, &Type_double, &val);
	return val;
}
