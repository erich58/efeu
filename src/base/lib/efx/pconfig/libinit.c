/*	Initialisierung der Programmbibliothek
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/procenv.h>
#include <EFEU/pconfig.h>
#include <EFEU/cmdeval.h>
#include <EFEU/parsedef.h>
#include <EFEU/object.h>
#include <EFEU/Info.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

/*	Systemabhängige Parameter:

S_SHELLENV	Environmentvariable mit Befehlsinterpreter.
S_SYSNAME	Systemname
*/

#define	S_SHELLENV	"SHELL"
#define	S_SYSNAME	"unix"


/*	Zusatzliche Variablen
*/

char *PS1 = "$!: ";
char *PS2 = "> ";
extern char *MessageHandler;

static char *VersionFmt = 
	"$! - $(Ident:%s)\n\tVersion $(Version:%s), $(Copyright:%s)\n";

char *UsageFmt = "\\Synopsis\n@synopsis\n";
char *HelpFmt = "\\manpage[1]{$!}\n\
\\Name\n@ident\n\
\\Synopsis\n@synopsis\n\
\\Description\n@arglist 10\n\
";

InfoNode_t *ProcInfo = NULL;

extern int ObjDebugFlag;

extern int float_align;
extern reftype_t lval_reftype;

static Var_t vardef[] = {
	{ "MakeDepend",	&Type_bool, &MakeDepend },
	{ "iodebug",	&Type_bool, &io_reftype.debug },
	{ "fdebug",	&Type_int, &FuncDebugFlag },
	{ "FuncDebug",	&Type_bool, &FuncRefType.debug },
	{ "VirFuncDebug",	&Type_bool, &VirFuncRefType.debug },
	{ "VarTabDebug",	&Type_bool, &VarTabRefType.debug },
	{ "odebug",	&Type_bool, &ObjDebugFlag },
	{ "ldebug",	&Type_bool, &lval_reftype.debug },
	{ "ProgIdent",	&Type_str, &ProgIdent },
	{ "ProgName",	&Type_str, &ProgName },
	{ "ApplPath",	&Type_str, &ApplPath },
	{ "InfoPath",	&Type_str, &InfoPath },
	{ "Shell",	&Type_str, &Shell },
	{ "Pager",	&Type_str, &Pager },
	{ "MessageHandler",	&Type_str, &MessageHandler },

	{ "PS1",	&Type_str, &PS1 },
	{ "PS2",	&Type_str, &PS2 },

	{ "UsageFmt",	&Type_str, &UsageFmt },
	{ "HelpFmt",	&Type_str, &HelpFmt },
	{ "VersionFmt",	&Type_str, &VersionFmt },

	{ "float_align",	&Type_int, &float_align },

	{ "global",	&Type_vtab, &GlobalVar },
	{ "context",	&Type_vtab, &ContextVar },

	{ "_allocdebug",	&Type_bool, &alloctab_debug_flag },
};


deftab_t PgmDefTab = {
	NULL,
	VB_DATA(8, sizeof(pardef_t)),
	VB_DATA(8, sizeof(pardef_t)),
	VB_DATA(8, sizeof(pardef_t)),
	VB_DATA(8, sizeof(pardef_t)),
	VB_DATA(8, sizeof(pardef_t)),
};


static void f_usage (Func_t *func, void *rval, void **arg);
static void f_iousage (Func_t *func, void *rval, void **arg);
static void f_expand (Func_t *func, void *rval, void **arg);

static Obj_t *p_interact (io_t *io, void *data);
static Obj_t *p_local (io_t *io, void *data);

static FuncDef_t fdef[] = {
	{ FUNC_VIRTUAL, &Type_void, "usage (IO def, str name)", f_usage },
	{ FUNC_VIRTUAL, &Type_void,  "usage (IO def, IO out)", f_iousage },
	{ 0, &Type_str,  "ExpandPath (str = NULL)", f_expand },
};

static ParseDef_t pdef[] = {
	{ "interactive", p_interact, NULL },
	{ "local", p_local, NULL },
};


/*	Globale Parameter
*/

static int need_init = 1;

static int c_font(io_t *in, io_t *out, void *arg)
{
	int n;

	n = io_ctrl(out, (int) (size_t) arg);
	return (n == EOF ? 0 : n);
}

static int c_name(io_t *in, io_t *out, void *arg)
{
	return io_puts(ProgName, out);
}

static int c_id(io_t *in, io_t *out, void *arg)
{
	return io_puts(ProgIdent, out);
}


/*	Initialisieren der Bibliotheksdefinitionen
*/

void libinit(const char *name)
{
	procinit(name);

	if	(need_init)
	{
		char *p;

		if	((p = getenv("EFEU")) != NULL)
			ApplPath = mstrcpy(p);

		psubfunc('!', c_name, NULL);
		psubfunc('%', c_id, NULL);
		psubfunc('R', c_font, (void *) IO_ROMAN_FONT);
		psubfunc('B', c_font, (void *) IO_BOLD_FONT);
		psubfunc('I', c_font, (void *) IO_ITALIC_FONT);
		SetupStd();
		AddFuncDef(fdef, tabsize(fdef));
		AddParseDef(pdef, tabsize(pdef));
		AddVar(NULL, vardef, tabsize(vardef));
		pconfig_init();
		applfile(MSG_EFMAIN, APPL_APP);
		pconfig_exit();
		need_init = 0;
	}
}


static void f_usage(Func_t *func, void *rval, void **arg)
{
	io_t *out;

	out = io_fileopen(*((char **) arg[1]), "w");
	cp_usage(Val_io(arg[0]), out);
	io_close(out);
}


static void f_iousage(Func_t *func, void *rval, void **arg)
{
	cp_usage(Val_io(arg[0]), Val_io(arg[1]));
}

static void f_expand(Func_t *func, void *rval, void **arg)
{
	Val_str(rval) = ExpandPath(Val_str(arg[0]));
}


static Obj_t *p_interact (io_t *io, void *data)
{
	char *prompt;
	char *hist;
	io_t *in;

	prompt = Obj2str(Parse_term(io, 0));
	hist = Obj2str(Parse_term(io, 0));
	in = io_interact(prompt, hist);
	CmdEval(in, iomsg);
	io_close(in);
	memfree(prompt);
	memfree(hist);
	return NULL;
}

static Obj_t *p_local (io_t *io, void *data)
{
	return NewPtrObj(&Type_vtab, rd_refer(LocalVar));
}

