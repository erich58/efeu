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
#include <EFEU/mactools.h>
#include <math.h>

#ifdef	NAN
#define	HAS_NAN	1
#endif

/*	Variablen
*/

static VarDef_t var_const[] = {
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

static Obj_t *p_eof (io_t *io, void *data)
{
	return int2Obj(EOF);
}


#if	HAS_NAN

static Obj_t *p_NaN (io_t *io, void *data)
{
	return double2Obj(NAN);
}

static Obj_t *p_Inf (io_t *io, void *data)
{
	return double2Obj(HUGE_VAL);
}

#endif

#ifndef	M_PI
#define M_PI        3.14159265358979323846264338327950288
#endif

static Obj_t *p_pi (io_t *io, void *data)
{
	return double2Obj(M_PI);
}


static ParseDef_t pdef_const[] = {
	{ "true", PFunc_bool, "" },
	{ "false", PFunc_bool, NULL },
	{ "EOF", p_eof, NULL },
	{ "FNAMESEP", PFunc_str, "/" },
	{ "PATHSEP", PFunc_str, ":" },
	{ "EFEUTOP", PFunc_str, String(_EFEU_TOP) },
	{ "EFEUSRC", PFunc_str, String(_EFEU_TOP) "/src" },
#if	HAS_NAN
	{ "NaN", p_NaN, NULL },
	{ "Inf", p_Inf, NULL },
#endif
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
