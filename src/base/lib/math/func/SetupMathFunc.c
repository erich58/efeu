/*
Mathematische Funktionen

$Copyright (C) 2002 Erich Frühstück
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
#include <Math/func.h>

#define	DRVAL	Val_double(rval)
#define	DARG(n)	Val_double(arg[n])

#define	DFUNC(name, expr)	\
static void name (EfiFunc *func, void *rval, void **arg) \
{ DRVAL = expr; }

/*	Funktionen
*/

DFUNC(f_dnormal, dnormal(DARG(0), DARG(1), DARG(2)))
DFUNC(f_pnormal, pnormal(DARG(0), DARG(1), DARG(2)))
DFUNC(f_qnormal, qnormal(DARG(0), DARG(1), DARG(2)))

DFUNC(f_dlognormal, dlognormal(DARG(0), DARG(1), DARG(2)))
DFUNC(f_plognormal, plognormal(DARG(0), DARG(1), DARG(2)))
DFUNC(f_qlognormal, qlognormal(DARG(0), DARG(1), DARG(2)))

static EfiFuncDef fdef[] = {
	{ 0, &Type_double,
		"dnormal (double x, double m = 0., double s = 1.)",
		f_dnormal },
	{ 0, &Type_double,
		"pnormal (double x, double m = 0., double s = 1.)",
		f_pnormal },
	{ 0, &Type_double,
		"qnormal (double x, double m = 0., double s = 1.)",
		f_qnormal },
	{ 0, &Type_double,
		"dlognormal (double x, double m = 0., double s = 1.)",
		f_dlognormal },
	{ 0, &Type_double,
		"plognormal (double x, double m = 0., double s = 1.)",
		f_plognormal },
	{ 0, &Type_double,
		"qlognormal (double x, double m = 0., double s = 1.)",
		f_qlognormal },
};


void SetupMathFunc(void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;

	AddFuncDef(fdef, tabsize(fdef));
}
