/*
Register und Lokaledefinitionen

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
#include <EFEU/cmdsetup.h>

static void f_reg_get(Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = mstrcpy(reg_get(Val_int(arg[0])));
}

static void f_reg_set(Func_t *func, void *rval, void **arg)
{
	reg_cpy(Val_int(arg[0]), Val_str(arg[1]));
}


/*	Initialisierung
*/

void CmdSetup_reg(void)
{
	SetFunc(FUNC_VIRTUAL, &Type_str, "operator#() (int)", f_reg_get);
	SetFunc(0, &Type_str, "getreg (int)", f_reg_get);
	SetFunc(0, &Type_void, "setreg (int, str)", f_reg_set);
}
