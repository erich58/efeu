/*	Register und Lokaledefinitionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
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
