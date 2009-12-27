/*	Konstante Objekte
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/locale.h>
#include <EFEU/mactools.h>
#include <math.h>

#if	_GNU_SOURCE
#define	HAS_NAN	1
#endif

/*	Variablen
*/

static Var_t var_const[] = {
	{ "iostd",	&Type_io, &iostd },
	{ "iomsg",	&Type_io, &iomsg },
	{ "ioerr",	&Type_io, &ioerr },
	{ "ionull",	&Type_io, &ionull },
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
	AddVar(NULL, var_const, tabsize(var_const));
	AddParseDef(pdef_const, tabsize(pdef_const));
}
