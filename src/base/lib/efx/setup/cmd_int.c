/*
Ganzzahlobjekte

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
#include <EFEU/cmdconfig.h>

#define	SIG	1
#define	S	"int"
#define	T	int

#define	ARITH	1

#include "stdint.src"

#define	CONV(name, type)	\
static void name (EfiFunc *func, void *rval, void **arg) \
{ *((type *) rval) = *((T *) arg[0]); }

CONV(c_int8, int8_t)
CONV(c_int16, int16_t)
CONV(c_int32, int32_t)
CONV(c_int64, int64_t)

CONV(c_uint, unsigned int)
CONV(c_uint8, uint8_t)
CONV(c_uint16, uint16_t)
CONV(c_uint32, uint32_t)
CONV(c_uint64, uint64_t)

static void to_str (EfiFunc *func, void *rval, void **arg)
{
	*((char **) rval) = msprintf("%d", *((T *) arg[0]));
}

static EfiFuncDef ctab[] = {
	{ FUNC_RESTRICTED, &Type_int8, S " ()", c_int8 },
	{ FUNC_RESTRICTED, &Type_int16, S " ()", c_int16 },
	{ FUNC_PROMOTION, &Type_int32, S " ()", c_int32 },
	{ FUNC_PROMOTION, &Type_int64, S " ()", c_int64 },
	{ FUNC_PROMOTION, &Type_varint, S " ()", c_int64 },

	{ FUNC_PROMOTION, &Type_uint, S " ()", c_uint },
	{ FUNC_RESTRICTED, &Type_uint8, S " ()", c_uint8 },
	{ FUNC_RESTRICTED, &Type_uint16, S " ()", c_uint16 },
	{ 0, &Type_uint32, S " ()", c_uint32 },
	{ 0, &Type_uint64, S " ()", c_uint64 },
	{ 0, &Type_varsize, S " ()", c_uint64 },
	
	{ 0, &Type_str, S " ()", to_str },
	{ 0, &Type_str, "str (" S ")", to_str },
};


void CmdSetup_int (void)
{
	AddFuncDef(ftab, tabsize(ftab));
	AddFuncDef(ctab, tabsize(ctab));
}
