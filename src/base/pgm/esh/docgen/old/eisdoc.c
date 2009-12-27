/*	Einmischen von Daten in TeX-Datenfiles
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"
#include <EFEU/Info.h>
#include <EFEU/Random.h>

#define	HNAME	"~/.edoc_history"
#define	HDR	"% Nicht editieren, Datei wurde mit $! generiert.\n"

/*	Globale Parameter
*/

extern char *IncPath;

static char *InputName = NULL;
static char *OutputName = NULL;
int VerboseMode = 1;
int MakeDepend = 0;

CmdTab_t *globtab = NULL;

static Var_t vardef[] = {
	{ "Input", &Type_str, &InputName },
	{ "Output", &Type_str, &OutputName },
	{ "VerboseMode", &Type_int, &VerboseMode },
	{ "MakeDepend", &Type_bool, &MakeDepend },
};

static Obj_t *cpar_expr (io_t *io, void *data)
{
	return Obj_call(data, NULL, NULL);
}

static Obj_t *cpar_cin (void *par, const ObjList_t *list)
{
	return LvalObj(&Type_io, NULL, &MergeStat.in);
}

static Obj_t *cpar_cout (void *par, const ObjList_t *list)
{
	return LvalObj(&Type_io, NULL, &MergeOut);
}

static Obj_t *cpar_ctrl (void *par, const ObjList_t *list)
{
	return LvalObj(&Type_io, NULL, &MergeStat.out);
}

static Obj_t *cpar_cmdkey (void *par, const ObjList_t *list)
{
	return LvalObj(&Type_char, NULL, &MergeStat.cmdkey);
}

static Obj_t *cpar_comkey (void *par, const ObjList_t *list)
{
	return LvalObj(&Type_char, NULL, &MergeStat.comkey);
}


static ParseDef_t parse_def[] = {
	{ "cin", cpar_expr, cpar_cin },
	{ "cout", cpar_expr, cpar_cout },
	{ "ctrl", cpar_expr, cpar_ctrl },
	{ "cmdkey", cpar_expr, cpar_cmdkey },
	{ "comkey", cpar_expr, cpar_comkey },
	{ "name", cpar_expr, cpar_name },
	{ "opt", cpar_expr, cpar_opt },
	{ "arg", cpar_expr, cpar_arg },
	{ "narg", cpar_expr, cpar_narg },
};

static void f_var(Func_t *func, void *rval, void **arg)
{
	char *p, *def;

	def = Val_str(arg[0]);

	if	(def && (p = strchr(def, '=')) != NULL)
	{
		*p = 0;
		SetMacro(AddCmd(globtab, def), "var", 0, p + 1);
		*p = '=';
	}
}

static FuncDef_t funcdef[] = {
	{ 0, &Type_void, "AddVar(str def)", f_var },
};

/*	Operator mit Zugriff auf die Mischbefehle
*/

static char *iomerge (io_t *in)
{
	strbuf_t *sb;
	Merge_t save;

	sb = new_strbuf(0);
	PushMerge(&save);
	MergeInput(rd_refer(in));
	MergeOutput(io_strbuf(sb));
	EvalMergeCmd(MergeStat.cmdkey);
	PopMerge(&save);
	return sb2str(sb);
}

static Obj_t *op_merge (io_t *io, Op_t *op, Obj_t *left)
{
	return str2Obj(iomerge(io));
}

static Op_t prefix_op[] = {
	{ "@", OpPrior_Unary, OpAssoc_Right, op_merge },
};

static int psub_merge (io_t *in, io_t *out, void *arg)
{
	char *s;
	int n;
	
	s = iomerge(in);
	n = io_puts(s, out);
	memfree(out);
	return n;
}

static void CmdInfo(io_t *io, InfoNode_t *info)
{
	ListCmdTab(io, info->par);
}

/*	Hauptprogramm
*/

int main(int narg, char **arg)
{
	io_t *in;

	libinit(arg[0]);
	SetupReadline();
	SetupMdMat();
	SetupPreproc();
	SetupUtil();
	SetupDataBase();
	SetupTimeSeries();
	SetupRand48();
	SetupRandom();
	SetupDebug();

	BuiltinCommand();
	AddParseDef(parse_def, tabsize(parse_def));
	AddOpDef(&PrefixTab, prefix_op, tabsize(prefix_op));
	AddFuncDef(funcdef, tabsize(funcdef));
	psubfunc('\\', psub_merge, NULL);
	globtab = NewCmdTab("global");
	AddInfo(NULL, "builtin", "Eingebaute Mischbefehle", CmdInfo, builtin);
	AddInfo(NULL, "global", "Globale Mischbefehle", CmdInfo, globtab);

	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	if	(OutputName == NULL)	MakeDepend = 0;

	if	(InputName)
	{
		in = io_fileopen(InputName, "r");
	}
	else	in = io_interact(NULL, HNAME);

	MergeInput(io_lnum(in));
	MergeOutput(io_fileopen(OutputName, "w"));
/*
	io_psub(merge.out, HDR);
*/
	MergeEval(0, EOF);
	MergeInput(NULL);
	MergeOutput(NULL);

	if	(MakeDepend && OutputName)
		MakeDependList(iostd, OutputName);

/*
	if	(VerboseMode)
		putc('\n', stderr);
*/

	rd_deref(globtab);
	return 0;
}
