/*	Einmischen von Daten in TeX-Datenfiles
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"
#include <EFEU/Op.h>

#ifndef	DEF_PATH
#define	DEF_PATH	"/local/lib/texmerge"
#endif


#if	UNIX_VERSION
#define	FMT_HEADER	"\nTeXMerge 1.0, Unix-Version\n"
#else
#define	FMT_HEADER	"\nTeXMerge 1.0, DOS-Version\n"
#endif

#define	FMT_CPR		"(c) 1996, Erich Frühstück\n\n"
#define	FMT_USAGE	"Aufruf: %s [-hldsvV] ein [aus] [var=value]\n"

#define	FMT_HELP	"\n\
\t-l\tBefehle auflisten.\n\
\t-d\tAbhängigkeitsregeln generieren.\n\
\t-s\tStille Verarbeitung.\n\
\t-v\tVollständiges Protokoll.\n\
\t-V\tReduziertes Protokoll.\n\
\n"

static void show_usage(void)
{
	printf(FMT_HEADER);
	printf(FMT_CPR);
	printf(FMT_USAGE, ProgramName);
	printf(FMT_HELP);
}

/*	Globale Parameter
*/

#if	UNIX_VERSION
extern char *IncPath;
#else
char *IncPath = NULL;
int MakeDepend = 0;
#endif

char *ProgramName = NULL;
int VerboseMode = 1;
int MergeMode = 1;

#if	UNIX_VERSION
static Var_t tm_var[] = {
	{ "VerboseMode", &Type_int, &VerboseMode },
};

static ParseDef_t tm_pdef[] = {
	/*
	{ "cout", Parse_cout, NULL },
	*/
	{ "opt", Parse_opt, NULL },
	{ "arg", Parse_arg, NULL },
	{ "narg", Parse_narg, NULL },
};

static char *iomerge (io_t *io)
{
	Input_t *in;
	Buffer_t *sb;

	in = OpenIOInput(io);
	sb = CreateBuffer();
	MergeBuf(in, sb);
	CloseInput(in);
	return CloseBuffer(sb);
}

static Obj_t *op_merge (io_t *io, Op_t *op, Obj_t *left)
{
	return str2Obj(iomerge(io));
}

static Op_t tm_prefix[] = {
	{ "@", OpPrior_Unary, OpAssoc_Right, op_merge },
};

static int psub_merge (io_t *in, io_t *out, void *arg)
{
	char *s;
	int n;
	
	s = iomerge(in);
	n = io_puts(s, out);
	FreeData("STR", out);
	return n;
}

#endif


/*	Hauptprogramm
*/

static char *ScriptName = NULL;
static char *OutputName = NULL;
static int args_set = 0;
static int list_only = 0;
extern int alloc_debug;

static void set_opt(int c)
{
	switch (c)
	{
	case 'A':	alloc_debug = 1; break;
	case 'd':	MakeDepend = 1; break;
	case 'v':	VerboseMode = 2; break;
	case 'V':	VerboseMode = 1; break;
	case 's':	VerboseMode = 0; break;
	case 'h':	show_usage(); exit(0);
	case 'l':	list_only = 1; break;
	default:	Error(1, c); exit(1);
	}
}

static void set_arg(char *arg)
{
	switch (args_set++)
	{
	case  0:	ScriptName = OutputName = arg; break;
	case  1:	OutputName = arg; break;
	default:	Error(2, arg); exit(1);
	}
}


int main(int narg, char **arg)
{

	int i, j;
	char *p;
	int check_option;

	ProgramName = arg[0];

#if	UNIX_VERSION
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
	AddVar(NULL, tm_var, tabsize(tm_var));
	AddParseDef(tm_pdef, tabsize(tm_pdef));
	AddOpDef(&PrefixTab, tm_prefix, tabsize(tm_prefix));
	psubfunc('@', psub_merge, NULL);
#endif
	IncPath = CopyString(getenv(CFG_ENV));

	if	(IncPath == NULL)
		IncPath = DEF_PATH;

	BuiltinCommand();
	args_set = 0;
	check_option = 1;

	for (i = 1; i < narg; i++)
	{
		if	(check_option && arg[i][0] == '-')
		{
			if	(arg[i][1] == 0)
			{
				set_arg(NULL);
			}
			else if	(arg[i][1] == '-' && arg[i][2] == 0)
			{
				check_option = 0;
			}
			else
			{
				for (j = 1; arg[i][j] != 0; j++)
					set_opt(arg[i][j]);
			}
		}
		else if	((p = strchr(arg[i], '=')) != NULL)
		{
			*p = 0;
			AddCommand(arg[i], TM_VAR, 0, p + 1);
		}
		else if	(arg[i][0] == '-' && arg[i][1] == 0)
		{
			set_arg(NULL);
		}
		else	set_arg(arg[i]);
	}

	if	(OutputName == NULL)	MakeDepend = 0;

#if	UNIX_VERSION
#else
	if	(MakeDepend)
		printf("%s.tex:", ScriptName);
#endif

	LoadConfig(NULL);

	if	(list_only)
	{
		ListCommand();
		exit(0);
	}

	if	(args_set == 0)
	{
		printf(FMT_USAGE, ProgramName);
		exit(1);
	}

	if	(ScriptName == NULL)
	{
		Input_t *in;
		Output_t *out;

		in = OpenInteractive();
		out = OpenTexfile(OutputName);
		Merge(in, out);
		CloseInput(in);
		CloseOutput(out);
	}
	else	TexMerge(ScriptName, OutputName);

	if	(MakeDepend)
#if	UNIX_VERSION
		MakeDependList(iostd, FileName(OutputName, TEX_EXT));
#else
		putchar('\n');
#endif

	if	(VerboseMode)
		putc('\n', stderr);

	return 0;
}
