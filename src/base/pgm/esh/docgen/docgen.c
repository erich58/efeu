/*	EIS-Dokumentationsgenerator
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdeval.h>
#include <EFEU/Info.h>
#include <EFEU/Document.h>

#define	HIST_NAME	"~/.docgen_history"
#define	CFG_NAME	"docgen"
#define	CFG_EXT		"mac"
#define	TYPE_EXT	"drv"
#define	MAC_EXT		"mac"

/*	Globale Parameter
*/

extern io_t *iolog;

static char *InputName = NULL;
static char *OutputName = NULL;
static char *EncodingName = NULL;
static char *TypeName = NULL;
static char *MacList = NULL;
static char *DocBeg = NULL;
static char *DocEnd = NULL;
int VerboseMode = 1;
int MakeDepend = 0;

static Var_t locvar[] = {
	{ "Input", &Type_str, &InputName },
	{ "Output", &Type_str, &OutputName },
	{ "DocBeg", &Type_str, &DocBeg },
	{ "DocEnd", &Type_str, &DocEnd },
	{ "Encoding", &Type_str, &EncodingName },
	{ "Type", &Type_str, &TypeName },
	{ "MacList", &Type_str, &MacList },
};

static Var_t globvar[] = {
	{ "VerboseMode", &Type_int, &VerboseMode },
	{ "MakeDepend", &Type_bool, &MakeDepend },
	{ "iolog",	&Type_io, &iolog },
};

static void f_macdef(Func_t *func, void *rval, void **arg)
{
	char *p, *def;

	def = Val_str(arg[0]);

	if	(def && (p = strchr(def, '=')) != NULL)
	{
		*p = 0;
		MergeTab_macro(MergeTabGlobal, mstrcpy(def),
			NULL, NULL, mstrcpy(p + 1));
		*p = '=';
	}
}

static FuncDef_t funcdef[] = {
	{ 0, &Type_void, "macdef(str def)", f_macdef },
};

static void preload (Merge_t *merge, const char *name, const char *ext)
{
	if	(name)
	{
		io_t *io = io_findopen(IncPath, name, ext, "r");

		if	(io != NULL)
		{
			io = io_cmdpreproc(io);
			Merge_ioeval(merge, io_lnum(io));
		}
		else	message(NULL, NULL, 1, 2, name, ext);
	}
}

/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	Merge_t *merge;
	io_t *in, *out;
	size_t i, mac_dim;
	char **mac_list;

	libinit(arg[0]);
	SetupReadline();
	SetupDocument();
	SetupDebug();

	AddFuncDef(funcdef, tabsize(funcdef));
	AddVar(NULL, globvar, tabsize(globvar));

	PushVarTab(NULL, NULL);
	pconfig(NULL, locvar, tabsize(locvar));
	loadarg(&narg, arg);
	PopVarTab();

	if	(OutputName == NULL)	MakeDepend = 0;

	if	(InputName)
	{
		in = io_lnum(io_fileopen(InputName, "r"));
	}
	else	in = io_interact(NULL, HIST_NAME);

	/*
	if	(EncodingName)
		in = inputenc(in, MergeEnc_get(EncodingName));
	*/

	in = io_cmdpreproc(in, NULL);

	if	(!VerboseMode)	iolog = NULL;

	if	(MakeDepend)
	{
		out = NULL;
	}
	else	out = io_fileopen(OutputName, "w");

	merge = Merge_open(out);
	preload(merge, CFG_NAME, CFG_EXT);
	preload(merge, TypeName, TYPE_EXT);

	mac_dim = strsplit(MacList, "%s", &mac_list);

	for (i = 0; i < mac_dim; i++)
		preload(merge, mac_list[i], MAC_EXT);

	memfree(mac_list);

	Merge_streval(merge, DocBeg);
	Merge_ioeval(merge, in);
	Merge_streval(merge, DocEnd);
	Merge_streval(merge, "\\at_exit\n");
	Merge_close(merge);

	if	(MakeDepend && OutputName)
		MakeDependList(iostd, OutputName);

	return 0;
}
