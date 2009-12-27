/*	Zählprogramme generieren
	(c) 1975 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_CNTPROG
#define	_MS_CNTPROG	1

#include <macro.ms>
#include <function.ms>
#include <countdef.ms>
#include <classdef.ms>
#include <eshvardef.ms>


/*	Eingabestruktur für Zählprogramm
*/

struct CountProgInput {
	str decl;
	str open;
	str close;
};

CountProgInput CountProgFileInput = {
	"FILE *input;",
	"input = fileopen(name, \"rz\");",
	"fileclose(input);",
};

CountProgInput CountProgIOInput = {
	"io_t *input;",
	"input = io_fileopen(name, \"rz\");",
	"io_close(input);",
};

CountProgInput CountProgInput(str decl, str open, str close)
{
	setreg(1, "input");
	setreg(2, "name");
	return { psub(decl), psub(open), psub(close) };
}

/*	Datenstruktur für Zählprogramm
*/

struct CountProgData {
	str decl;
	str init;
	str clean;
};

CountProgData CountProgVoidData = {
	"void * data;",
	NULL,
	NULL,
};

CountProgData CountProgBufferData(str type)
	return { type + " buffer, *data;", "data = &buffer;", NULL }

CountProgData CountProgData(str decl, str init = NULL, str clean = NULL)
{
	setreg(1, "data");
	return { psub(decl), psub(init), psub(clean) };
}


/*	Zählprogrammobjekt
*/

struct CountProg {
	str name;
	str vdef;
	str init;
	CountProgInput input;
	CountProgData data;
	str eval;
};


CountProg CountProg(str name)
	return { name, NULL, NULL,
		CountProgFileInput, CountProgVoidData, NULL }


/*	Zähler eintragen
*/

virtual void CountProg::countdef & (str name)
	this.init += sprintf("%s(&%sCountTab);\n", name, this.name);

virtual void CountProg::countdef & (str name, CountDef def)
{
	str tabname = name + "Tab";
	def.tab(tabname);
	this.init += sprintf("XAPPEND(&%sCountTab, %s, XS_ENTER);\n",
		this.name, tabname);
}


/*	KLassifikation eintragen
*/

virtual void CountProg::classdef & (str name)
	this.init += sprintf("%s(&%sClassTab);\n", name, this.name);

virtual void CountProg::classdef & (str cname, ClassDef def)
{
	def.tab(cname);
	init += sprintf("XAPPEND(&%sClassTab, %s_tab, XS_ENTER);\n", name, cname);
}

/*	Initialisierungsfunktion
*/

void CountProg::setup(bool global = false)
{
	setreg(1, this.name);
	fpsub(src, string !

/*	Zählprogramm $1
*/

static XTAB($1ClassTab, 0, skey_cmp);
static XTAB($1CountTab, 0, skey_cmp);

!);
	Function f;

	f = Function("void $1ShowClass",
		"Func_t *func, void *rval, void **arg");
	f.line("MdShowClass(iostd, &$1ClassTab, Val_str(arg[0]));");
	f.line("libexit(EXIT_SUCCESS);");
	f.write(false);

	f = Function("void $1ShowCount",
		"Func_t *func, void *rval, void **arg");
	f.line("md_showcnt(iostd, &$1CountTab);");
	f.line("libexit(EXIT_SUCCESS);");
	f.write(false);

	fpsub(src, string !

static FuncDef_t $1FuncDef[] = {
	{ 0, &Type_void, "ShowClass (str = NULL)", $1ShowClass },
	{ 0, &Type_void, "ShowCount ()", $1ShowCount },
};

!);
	f = Function("void Setup$1");
	f.line("AddFuncDef($1FuncDef, tabsize($1FuncDef));");
	f.body(this.init);
	f.write(global);
}


/*	Zählstruktur generieren
*/

void CountProg::count(str read, bool global = false)
{
	setreg(1, this.name);
	setreg(2, "input");
	setreg(3, "data");
	Function f = Function("mdmat_t *Count$1",
		"const char *name, const char *title, const char *cnt, const char *list");
	f.body(this.input.decl);
	f.body(this.data.decl);
	f.line("mdmat_t *md;");
	f.line();
	f.body(this.input.open);
	f.body(this.data.init);
	f.line("md = md_ctab(title, list, &$1ClassTab, skey_find(&$1CountTab, cnt));");
	f.line();
	f.line("while ((" + psub(read) + "))");

	if	(this.eval)
	{
		f.beg();
		f.body(this.eval)
		f.end();
	}
	else	f.line("\tmd_count(md, $3);");

	f.line();
	f.body(this.input.close);
	f.body(this.data.clean);
	f.line("return md;");
	f.write(global);
}


void CountProg::vardef()
{
	setreg(1, this.name);
	src << "\n/*\tVariablen\n*/\n";

	EshVarDef v = EshVarDef("vardef");
	v.load(string !
str	Title	$(#1:%#s)
str	Input	NULL
str	Output	NULL
str	Counter	NULL
str	Grpdef	NULL
!);
	v.load(this.vdef);
	v.write();
}

void CountProg::main()
{
	setreg(1, this.name);
	src << "\n/*\tHauptprogramm\n*/\n";

	Function f = Function("int main", "int narg, char **arg")
	f.line("libinit(arg[0]);");
	f.line("SetupMdMat();");
	f.line("Setup$1();");
	f.line("pconfig(NULL, vardef, tabsize(vardef));");
	f.line("loadarg(&narg, arg);");
	f.line("md_fsave(Output, Count$1(Input, Title, Counter, Grpdef), 0);");
	f.line("return 0;");
	f.write(true);
}

#endif	/* _MS_CNTPROG */
