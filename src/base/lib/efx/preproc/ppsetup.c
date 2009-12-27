/*	Eingabe - Preprozessor
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>
#include <EFEU/mactools.h>
#include <EFEU/ioctrl.h>

/*	Variablen
*/

char *IncPath = ".:" String(_EFEU_TOP) "/lib/esh";

static Var_t pp_var[] = {
	{ "IncPath",	&Type_str, &IncPath },
};

#define	RVIO	Val_io(rval)

#define	STR(n)	Val_str(arg[n])
#define	IO(n)	Val_io(arg[n])

static void f_preproc (Func_t *func, void *rval, void **arg)
{
	RVIO = io_cmdpreproc(io_refer(IO(0)));
}

static void f_ppopen (Func_t *func, void *rval, void **arg)
{
	RVIO = io_cmdpreproc(io_findopen(IncPath, STR(0), STR(1), "r"));
}

static void f_lastcomment (Func_t *func, void *rval, void **arg)
{
	io_ctrl(IO(0), IOPP_COMMENT, rval);
}

static void f_clearcomment (Func_t *func, void *rval, void **arg)
{
	io_ctrl(IO(0), IOPP_COMMENT, NULL);
}

static FuncDef_t ppfdef[] = {
	{ 0, &Type_io, "preproc (IO io)", f_preproc },
	{ 0, &Type_io, "ppopen (str name, str type = NULL)",
		f_ppopen },
	{ 0, &Type_str, "lastcomment (IO io = cin)", f_lastcomment },
	{ 0, &Type_str, "clearcomment (IO io = cin)", f_clearcomment },
	{ 0, &Type_void, "target (str)", Func_target },
	{ 0, &Type_void, "depend (str)", Func_depend },
	{ 0, &Type_void, "makedepend (str, IO = iostd)", Func_makedepend },
	{ 0, &Type_list, "dependlist ()", Func_dependlist },
	{ 0, &Type_list, "targetlist ()", Func_targetlist },
};

void SetupPreproc()
{
	AddVar(GlobalVar, pp_var, tabsize(pp_var));
	AddFuncDef(ppfdef, tabsize(ppfdef));
}
