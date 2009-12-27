/*
Gleitkommaobjekte

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
#include <ctype.h>


#define	S	"double"
#define	T	double

#include "float.src"

/*	Zusätzliche Funktionen
*/

static void f_xdiv (EfiFunc *func, void *rval, void **arg)
{
	T a = *((T *) arg[0]);
	T b = *((T *) arg[1]);
	*((T *) rval) = b ? a / b : 0.;
}

static void f_xdiv1 (EfiFunc *func, void *rval, void **arg)
{
	T a = *((T *) arg[0]);
	T b = *((T *) arg[1]);
	*((T *) rval) = b ? a / b : 1.;
}

static void f_idx (EfiFunc *func, void *rval, void **arg)
{
	T a = *((T *) arg[0]);
	T b = *((T *) arg[1]);
	*((T *) rval) = b ? 100. * a / b : 0.;
}

static void f_rate (EfiFunc *func, void *rval, void **arg)
{
	T a = *((T *) arg[0]);
	T b = *((T *) arg[1]);
	*((T *) rval) = b ? 100. * (a - b) / b : 0.;
}

static void f_rnd (EfiFunc *func, void *rval, void **arg)
{
	T a = *((T *) arg[0]);
	*((int *) rval) = a < 0. ? - (int) (-a + 0.5) : (int) (a + 0.5);
}


static EfiFuncDef ftab_ext[] = {
	{ 0, NULL, S " xdiv (" S ", " S ")", f_xdiv },
	{ 0, NULL, S " xdiv1 (" S ", " S ")", f_xdiv1 },
	{ 0, NULL, S " idx (" S ", " S ")", f_idx },
	{ 0, NULL, S " rate (" S ", " S ")", f_rate },
	{ 0, &Type_int, "rnd (" S ")", f_rnd },
};

/*	Initialisierung
*/

void CmdSetup_double(void)
{
	AddFuncDef(ftab, tabsize(ftab));
	AddFuncDef(ftab_ext, tabsize(ftab_ext));
}
