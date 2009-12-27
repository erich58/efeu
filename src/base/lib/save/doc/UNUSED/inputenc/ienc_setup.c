/*	Initialisierung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/InputEnc.h>

Type_t Type_InputEnc = REF_TYPE("InputEnc", InputEnc_t *);


static void ie_create (Func_t *func, void *rval, void **arg)
{
	Val_ptr(rval) = InputEnc(Val_str(arg[0]));
}

static void ie_load (Func_t *func, void *rval, void **arg)
{
	register InputEnc_t *tab = rd_refer(Val_ptr(arg[0]));
	InputEnc_load(tab, Val_io(arg[1]));
	Val_ptr(rval) = tab;
}

static void ie_print (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = InputEnc_print(Val_io(arg[0]), Val_ptr(arg[1]));
}

static FuncDef_t stdfunc[] = {
	{ 0, &Type_InputEnc, "InputEnc (str name)", ie_create },
	{ 0, &Type_InputEnc, "InputEnc::load (IO io)", ie_load },
	{ FUNC_VIRTUAL, &Type_int, "fprint (IO io, InputEnc ie)", ie_print },
};

void SetupInputEnc (void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	AddType(&Type_InputEnc);
	AddFuncDef(stdfunc, tabsize(stdfunc));
}
