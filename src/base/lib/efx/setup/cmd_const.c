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
#include <EFEU/locale.h>
#include <EFEU/math.h>

/*	Variablen
*/

static EfiVarDef var_const[] = {
	{ "iostd",	&Type_io, &iostd,
		":*:standard input output\n"
		":de:Standarein/ausgabestruktor\n" },
	{ "iomsg",	&Type_io, &iomsg,
		":*:message output (stdout)\n"
		":de:Ausgabe von Meldungen (stdout)\n" },
	{ "ioerr",	&Type_io, &ioerr,
		":*:error output (stderr)\n"
		":de:Ausgabe von Fehlern (stderr)\n" },
	{ "ionull",	&Type_io, &ionull,
		":*:null input/output\n"
		":de:Leere Ein/Ausgabestruktur\n" },
};

/*	Schlüsselwörter
*/

static EfiObj *p_eof (IO *io, void *data)
{
	return int2Obj(EOF);
}

static EfiObj *p_NaN (IO *io, void *data)
{
	return double2Obj(ExceptionValue(0.));
}

static EfiObj *p_Inf (IO *io, void *data)
{
	return double2Obj(ExceptionValue(1.));
}

static EfiObj *p_pi (IO *io, void *data)
{
	return double2Obj(M_PI);
}

static EfiParseDef pdef_const[] = {
	{ "true", PFunc_bool, "" },
	{ "false", PFunc_bool, NULL },
	{ "EOF", p_eof, NULL },
	{ "FNAMESEP", PFunc_str, "/" },
	{ "PATHSEP", PFunc_str, ":" },
	{ "NaN", p_NaN, NULL },
	{ "Inf", p_Inf, NULL },
	{ "M_PI", p_pi, NULL },
	{ "LC_SCAN", PFunc_int, (void *) LOC_SCAN },
	{ "LC_PRINT", PFunc_int, (void *) LOC_PRINT },
	{ "LC_DATE", PFunc_int, (void *) LOC_DATE },
	{ "LC_ALL", PFunc_int, (void *) LOC_ALL },
};

void CmdSetup_const(void)
{
	AddVarDef(NULL, var_const, tabsize(var_const));
	AddParseDef(pdef_const, tabsize(pdef_const));
}
