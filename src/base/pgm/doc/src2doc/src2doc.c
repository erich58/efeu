/*	Dokumentation aus Sourcefile generieren
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include <EFEU/pconfig.h>
#include <EFEU/strbuf.h>
#include "src2doc.h"
#include <ctype.h>


/*	Resourcedefinitionen
*/

char *Input = NULL;
char *Output = NULL;
char *Mode = NULL;
char *Name = NULL;
char *Secnum = NULL;

Var_t vardef[] = {
	{ "Input",	&Type_str, &Input },
	{ "Output",	&Type_str, &Output },
	{ "Mode",	&Type_str, &Mode },
	{ "Name",	&Type_str, &Name },
	{ "Secnum",	&Type_str, &Secnum },
};

static void f_list (Func_t *func, void *rval, void **arg)
{
	S2DMode_list(Val_io(arg[0]));
}

static FuncDef_t fdef[] = {
	{ FUNC_VIRTUAL, &Type_void, "ListMode (IO io = iostd)", f_list },
};


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	io_t *ein;
	io_t *aus;
	S2DEval_t eval;

	libinit(arg[0]);
	SetupReadline();
	AddFuncDef(fdef, tabsize(fdef));
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	eval = Mode ? S2DMode_get(Mode) : S2DName_get(Input);

	if	(eval == NULL)
	{
		message(NULL, NULL, 1, 1, Mode);
		libexit(EXIT_FAILURE);
	}

	if	(!Name && Input)
	{
		fname_t *fn = strtofn(Input);

		if	(fn)
		{
			fn->path = NULL;
			Name = fntostr(fn);
			memfree(fn);
		}
	}

	ein = Input ? io_fileopen(Input, "rb") : io_interact(NULL, "s2d_hist");
	ein = io_lnum(ein);
	aus = io_fileopen(Output, "wb");
	eval(Name, ein, aus);
	io_close(ein);
	io_close(aus);
	return 0;
}
