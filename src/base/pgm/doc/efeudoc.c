/*	Dokumentgenerator
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Readline.h>
#include <EFEU/calendar.h>
#include "efeudoc.h"
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <EFEU/object.h>

#define	CFNAME	"efeudoc"
#define	HNAME	"~/.doc_history"
#define	HEAD	"Dokumenteingabe: $0"

char *Config = NULL;
char *Type = NULL;
char *Input = NULL;
char *Output = NULL;
char *Head = NULL;
int NoHead = 0;

#define	DATE_FMT	"Letzte Änderung: %d. %d. %d %d:%02d"
#define	DATE_VAR(x)	x->tm_mday, x->tm_mon + 1, x->tm_year + 1900, \
			x->tm_hour, x->tm_min
#define	USER_FMT	"Eigentümer: %s (%s)"
#define	USER_VAR(x)	x->pw_name, x->pw_gecos

Var_t locvar[] = {
	{ "Config", &Type_str, &Config },
	{ "Type", &Type_str, &Type },
	{ "Input", &Type_str, &Input },
	{ "Output", &Type_str, &Output },
	{ "Head", &Type_str, &Head },
	{ "NoHead", &Type_bool, &NoHead },
};

static void f_setmac (Func_t *func, void *rval, void **arg)
{
	char *name, *def;
	name = Val_str(arg[0]);

	if	(name == NULL)	return;

	def = strchr(name, '=');

	if	(def)
	{
		name = mstrncpy(name, def - name);
		def = mstrcpy(def + 1);
	}
	else	name = mstrcpy(name);

	DocTab_setmac(GlobalDocTab, name, NULL, def);
}

static FuncDef_t locfunc[] = {
	{ 0, &Type_void, "MacDef (str def)", f_setmac },
};

/*	Globale Variablen
*/

char *DocPath = NULL;
char *DocName = NULL;
char *DocInit = NULL;
char *DocExit = NULL;

Var_t globvar[] = {
	{ "DocPath", &Type_str, &DocPath },
	{ "DocName", &Type_str, &DocName },
	{ "DocInit", &Type_str, &DocInit },
	{ "DocExit", &Type_str, &DocExit },
};


static void eval_file (Doc_t *doc, const char *path, const char *name)
{
	io_t *in;
	char *p;

	in = name ? Doc_open(path, name, 1) : io_interact(NULL, HNAME);
	reg_set(0, io_ident(in));
	p = parsub(HEAD);
	io_ctrl(doc->out, DOC_REM, p);
	memfree(p);
	Doc_copy(doc, in);
	io_close(in);
}


int main (int narg, char **arg)
{
	Doc_t *doc = NULL;
	int i;

	libinit(arg[0]);
	AddVar(NULL, globvar, tabsize(globvar));
	SetupDoc();
	SetupReadline();
	SetupDebug();
	DocTab_fload(GlobalDocTab, CFNAME);
	DocOutInfo("DocOut", "Dokumentausgabefilter");
	DocMacInfo("DocMac", "Makrodefinitionen");
	PushVarTab(NULL, NULL);
	pconfig(NULL, locvar, tabsize(locvar));
	AddFuncDef(locfunc, tabsize(locfunc));
	loadarg(&narg, arg);
	PopVarTab();

	if	(narg <= 1)
	{
		usage(UsageFmt, ioerr);
		libexit(EXIT_FAILURE);
	}

	if	(getuid() == 0)
	{
		message(NULL, MSG_DOC, 0, 0);
		return 1;
	}

	DocTab_fload(GlobalDocTab, Config);
	doc = Doc(Type);
	doc->out = DocOut(doc->type, Output);
	doc->stat = NoHead;
	Doc_eval(doc, NULL, DocInit);

	if	(Head)
	{
		io_t *in = io_cstr(Head);
		Doc_copy(doc, in);
		io_close(in);
	}

	for (i = 1; i < narg; i++)
	{
		char *p = argval(arg[i]);
		eval_file(doc, CFGPATH, p);
		memfree(p);
	}

	Doc_eval(doc, NULL, DocExit);
	Doc_endall(doc, 0);
	rd_deref(doc);
	return 0;
}
