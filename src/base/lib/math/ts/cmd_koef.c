/*	Regressionskoeffizienten
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <EFEU/stdtype.h>
#include <Math/TimeSeries.h>


/*	Datentype
*/

static void Clean_OLSKoef(const Type_t *st, void *data)
{
	memfree(Val_OLSKoef(data).name);
	Val_OLSKoef(data).name = NULL;
}

static void Copy_OLSKoef(const Type_t *st, void *tg, const void *src)
{
	Val_OLSKoef(tg).name = mstrcpy(Val_OLSKoef(src).name);
	Val_OLSKoef(tg).val = Val_OLSKoef(src).val;
	Val_OLSKoef(tg).se = Val_OLSKoef(src).se;
}


Type_t Type_OLSKoef = STD_TYPE("OLSKoef", OLSKoef_t, NULL,
	Clean_OLSKoef, Copy_OLSKoef);


/*	Komponentenfunktionen
*/

static char **koef_name (OLSKoef_t *koef)
{
	return &koef->name;
}

static double *koef_val (OLSKoef_t *koef)
{
	return &koef->val;
}

static double *koef_se (OLSKoef_t *koef)
{
	return &koef->se;
}

static double *koef_t (OLSKoef_t *koef)
{
	Buf_double = koef->se ? koef->val / koef->se : 0.;
	return &Buf_double;
}

static Var_t var_OLSKoef[] = {
	{ "name", &Type_str, NULL, 0, 0, LvalMember, koef_name },
	{ "val", &Type_double, NULL, 0, 0, LvalMember, koef_val },
	{ "se", &Type_double, NULL, 0, 0, LvalMember, koef_se },
	{ "t", &Type_double, NULL, 0, 0, ConstMember, koef_t },
};


/*	Funktionen
*/

static void f_fprint (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = PrintOLSKoef(Val_io(arg[0]), arg[1]);
}


/*	Initialisieren
*/

static FuncDef_t func[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, OLSKoef)", f_fprint },
};


void CmdSetup_OLSKoef (void)
{
	AddType(&Type_OLSKoef);
	AddFuncDef(func, tabsize(func));
	AddVar(Type_OLSKoef.vtab, var_OLSKoef, tabsize(var_OLSKoef));
}
