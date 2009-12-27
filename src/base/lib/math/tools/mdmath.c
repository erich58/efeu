/*	MdMat - Funktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/object.h>
#include <Math/mdmath.h>


static void f_mdinv (Func_t *func, void *rval, void **arg)
{
	Val_mdmat(rval) = mdinv(Val_mdmat(arg[0]));
}

static FuncDef_t fdef[] = {
	{ 0, &Type_mdmat, "mdinv (mdmat a)", f_mdinv },
};

void SetupMdMath(void)
{
	static int need_setup = 1;

	if	(need_setup)
	{
		AddFuncDef(fdef, tabsize(fdef));
		need_setup = 0;
	}
}
