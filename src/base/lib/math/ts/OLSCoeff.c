/*
:*:regression coefficents
:de:Regressionskoeffizienten

$Copyright (C) 1997 Erich Frühstück
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
#include <Math/TimeSeries.h>


/*	Datentype
*/

static void Clean_OLSCoeff(const EfiType *st, void *data)
{
	memfree(Val_OLSCoeff(data).name);
	Val_OLSCoeff(data).name = NULL;
}

static void Copy_OLSCoeff(const EfiType *st, void *tg, const void *src)
{
	Val_OLSCoeff(tg).name = mstrcpy(Val_OLSCoeff(src).name);
	Val_OLSCoeff(tg).val = Val_OLSCoeff(src).val;
	Val_OLSCoeff(tg).se = Val_OLSCoeff(src).se;
}


EfiType Type_OLSCoeff = STD_TYPE("OLSCoeff", OLSCoeff, NULL,
	Clean_OLSCoeff, Copy_OLSCoeff);


/*	Komponentenfunktionen
*/

static EfiObj *koef_name (const EfiObj *base, void *data)
{
	if	(base)
	{
		OLSCoeff *koef = base->data;
		return LvalObj(&Lval_obj, &Type_str, base, &koef->name);
	}

	return NULL;
}

static EfiObj *koef_val (const EfiObj *base, void *data)
{
	if	(base)
	{
		OLSCoeff *koef = base->data;
		return LvalObj(&Lval_obj, &Type_double, base, &koef->val);
	}

	return NULL;
}

static EfiObj *koef_se (const EfiObj *base, void *data)
{
	if	(base)
	{
		OLSCoeff *koef = base->data;
		return LvalObj(&Lval_obj, &Type_double, base, &koef->se);
	}

	return NULL;
}

static EfiObj *koef_tstat (const EfiObj *base, void *data)
{
	if	(base)
	{
		OLSCoeff *koef = base->data;
		/*
		double t = koef->se ? koef->val / koef->se : 0.;
		*/
		double t = koef->val / koef->se;
		return NewObj(&Type_double, &t);
	}

	return NULL;
}

static EfiMember var_OLSCoeff[] = {
	{ "name", &Type_str, koef_name, NULL },
	{ "val", &Type_double, koef_val, NULL },
	{ "se", &Type_double, koef_se, NULL },
	{ "t", &Type_double, koef_tstat, NULL },
};


/*	Funktionen
*/

static void f_fprint (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = OLSCoeff_print(Val_io(arg[0]), arg[1]);
}


/*	Initialisieren
*/

static EfiFuncDef func[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, OLSCoeff)", f_fprint },
};


void CmdSetup_OLSCoeff (void)
{
	AddType(&Type_OLSCoeff);
	AddFuncDef(func, tabsize(func));
	AddEfiMember(Type_OLSCoeff.vtab, var_OLSCoeff, tabsize(var_OLSCoeff));
}

#define CFMT	"%-9.9s %15.6g %15.6g %15.6g\n"

int OLSCoeff_print (IO *io, OLSCoeff *koef)
{
	/*
	return io_printf(io, CFMT, koef->name,
		koef->val, koef->se, koef->se ? koef->val / koef->se : 0.);
	*/
	return io_printf(io, CFMT, koef->name,
		koef->val, koef->se, koef->val / koef->se);
}
