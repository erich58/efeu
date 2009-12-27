/*
MdMat - Funktionen

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
#include <Math/mdmath.h>


static void f_mdinv (EfiFunc *func, void *rval, void **arg)
{
	Val_mdmat(rval) = mdinv(Val_mdmat(arg[0]));
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_mdmat, "mdinv (mdmat a)", f_mdinv },
};

void SetupMdMath (void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;
	AddFuncDef(fdef, tabsize(fdef));
}
