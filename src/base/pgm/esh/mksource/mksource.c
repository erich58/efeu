/*
:*:program generator
:de:Programmgenerierung

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/Resource.h>
#include <EFEU/pconfig.h>
#include <EFEU/parsedef.h>
#include <EFEU/preproc.h>
#include <EFEU/efutil.h>
#include <EFEU/database.h>
#include <EFEU/mdmat.h>
#include <EFEU/Random.h>
#include <EFEU/MakeDepend.h>
#include <EFEU/cmdeval.h>
#include <EFEU/ioctrl.h>
#include <Math/TimeSeries.h>
#include <EFEU/StatData.h>
#include <Math/pnom.h>
#include <Math/mdmath.h>
#include <Math/func.h>
#include <ctype.h>

#define	PROTECTKEY	"ms_"
#define	NAME		"name"

#define	F_HEAD	":*:Do not edit, file was created with\n" \
		":de:Nicht editieren, Datei wurde mit\n"
#define	F_END	":de:generiert.\n"

#define	R_HEAD	"\n# Rules created with\n# $0\n"
#define	R_END	"\n# End of rules created with $!\n"


/*	Globale Variablen
*/

static char *BaseName = NULL;	/* Basisname */
static int Verbose = 0;		/* Protokollmodus */
static char *bootstrap = NULL;	/* Aufrufparameter */
static int hdr_comment = 0;	/* Kommentare auch in den Header schreiben */
static SB_DECL(sb_flags, 0);	/* Buffer für Flags */

static void put_comment (const char *txt, IO *out)
{
	io_puts("\n/*\n", out);
	io_puts(txt, out);
	io_puts("*/\n\n", out);
}

/*	Ausgabedefinitionen
*/

static void hdr_c (IO *out, const char *txt)
{
	static int need_nl;

	io_puts("/*\n", out);

	if	(txt)
	{
		for (need_nl = 0; *txt; txt++)
		{
			io_putc(*txt, out);
			need_nl = *txt != '\n';
		}

		if	(need_nl)
			io_putc('\n', out);

		io_putc('\n', out);
	}

	io_langputs(F_HEAD, out);
	io_puts(bootstrap, out);
	io_putc('\n', out);
	io_langputs(F_END, out);
	io_puts("*/\n\n", out);
}

static void hdr_tex (IO *out, const char *txt)
{
	static int at_start;

	if	(!txt)	return;

	for (at_start = 1; *txt; txt++)
	{
		if	(at_start)
			io_puts("% ", out);

		io_putc(*txt, out);
		at_start = (*txt == '\n');
	}

	if	(!at_start)
		io_putc('\n', out);

	io_putc('\n', out);
}

typedef struct {
	char *name;	/* Name */
	char *ext;	/* Filezusatz */
	void (*hfunc) (IO *out, const char *txt);
	int protect;	/* Schutzflag */
	char *fname;	/* Filename */
	int flag;	/* Steuerflag */
	IO *io;		/* IO - Struktur */
	int inc;	/* Flag für eingebundene Dateien */
} OUTPUT;

static OUTPUT output[] = {
	{ "hdr", "h", hdr_c, 1, NULL, 0, NULL, 0 },
	{ "top", NULL, NULL, 0, NULL, 0, NULL, 0 },
	{ "src", "c", hdr_c, 0, NULL, 0, NULL, 0 },
	{ "tex", "tex", hdr_tex, 0, NULL, 0, NULL, 0 },
	{ "doc", "doc", hdr_c, 0, NULL, 0, NULL, 0 },
	{ "info", "info", hdr_c, 0, NULL, 0, NULL, 0 },
};

#define	OUT_HDR	0
#define	OUT_TOP	1
#define	OUT_SRC	2

static void divide_src (void)
{
	if	(!output[OUT_SRC].io)	return;

	if	(!output[OUT_HDR].io)
	{
		if	(Verbose)
			io_xprintf(ioerr, "%s == %s\n", output[OUT_HDR].name,
				output[OUT_SRC].name);

		output[OUT_HDR].io = output[OUT_SRC].io;
		output[OUT_SRC].io = io_divide(output[OUT_HDR].io, 0);
	}
	else	hdr_comment = 1;

	if	(!output[OUT_TOP].io)
	{
		output[OUT_TOP].io = output[OUT_SRC].io;
		output[OUT_SRC].io = io_divide(output[OUT_TOP].io, 0);
	}
}

static OUTPUT *get_output(const char *name)
{
	int i;

	for (i = 0; i < tabsize(output); i++)
		if	(output[i].ext && mstrcmp(name, output[i].name) == 0)
			return output + i;

	return NULL;
}

static EfiVarDef globvar[] = {
	{ "HeaderName",		&Type_str, &output[OUT_HDR].fname },
	{ "BaseName",		&Type_str, &BaseName },
	{ "VerboseMode",	&Type_int, &Verbose },
};

/*	Lokale Variablen für Aufrufargumente
*/

static char *AllTarget = "all";	/* Generierungsziel */
static char *CleanTarget = "clean";	/* Aufräumziel */
static char *DependTarget = "depend";	/* Abhängigkeitsziel */
static char *Template = NULL;	/* Eingabefile */
static char *DependName = NULL;	/* Abhängigkeitsname */
static char *ListName = NULL;	/* Listendatei */
static int MakeDep = 0;		/* Abhängigkeitsregeln generieren */
static int MakeRule = 0;	/* Regel generieren */
static int MakeList = 0;	/* Liste generieren */
static int LockFlag = 0;	/* Sperrflag */

static EfiVarDef locvar[] = {
	{ "Template",	&Type_str, &Template },
	{ "DependName",	&Type_str, &DependName },
	{ "ListName",	&Type_str, &ListName },
	{ "AllTarget",	&Type_str, &AllTarget },
	{ "CleanTarget",	&Type_str, &CleanTarget },
	{ "DependTarget",	&Type_str, &DependTarget },
	{ "MakeRule",	&Type_bool, &MakeRule },
	{ "MakeList",	&Type_bool, &MakeList },
	{ "MakeDep",	&Type_bool, &MakeDep },
	{ "LockFlag",	&Type_bool, &LockFlag },
};


static void protect_name (const char *name, IO *io)
{
	io_puts(PROTECTKEY, io);

	for (; *name != 0; name++)
		io_putc(isalnum(*name) ? *name : '_', io);
}


/*	Ausgabefiles öffnen/schließen
*/

static int SetupOutput = 1;

static void open_output (OUTPUT *out, const char *name, const char *head)
{
	if	(out->io)	return;

	if	(SetupOutput || !LockFlag)
		out->flag = 1;

	if	(out->fname == NULL)
	{
		if	(name)
			out->fname = mstrcpy(name);
		else if	(!SetupOutput)
			out->fname = mstrcat(".", BaseName, out->ext, NULL);
	}

	if	(!out->flag)	return;

	AddTarget(out->fname);

	if	(SetupOutput || MakeDep || MakeRule || MakeList)
		return;

	if	(Verbose)
		io_xprintf(ioerr, "%s > %s\n", out->name, out->fname);

	out->io = io_fileopen(out->fname, "w");

	if	(out->hfunc)
		out->hfunc(out->io, head);

	if	(out && out->protect && out->fname)
	{
		io_puts("#ifndef\t", out->io);
		protect_name(out->fname, out->io);
		io_puts("\n#define\t", out->io);
		protect_name(out->fname, out->io);
		io_puts("\t1\n\n", out->io);
	}
}

static void close_output (OUTPUT *out)
{
	if	(!out->fname)	out->flag = 0;
	if	(!out->io)	return;

	if	(out->inc)
		rd_deref(io_pop(out->io));

	if	(out->protect && out->fname)
	{
		io_puts("\n#endif\t/* ", out->io);
		protect_name(out->fname, out->io);
		io_puts(" */\n", out->io);
	}

	io_close(out->io);
	out->io = NULL;
	out->inc = 0;
}


/*	Ausgabedefinition initialisieren
*/

static void add_output(OUTPUT *out)
{
	EfiObj *obj = LvalObj(&Lval_ptr, &Type_io, &out->io);
	VarTab_xadd(NULL, mstrcpy(out->name), NULL, obj);
}

/*	Konfiguration
*/

static int eval_stat = 1;

static EfiObj *p_config (IO *io, void *data)
{
	char *name, *p, *arg;
	char *head;
	OUTPUT *out;
	int c;
	
	io_ctrl(io, IOPP_COMMENT, &head);

	while ((c = io_eat(io, " \t")) != EOF)
	{
		if	(c == '\n')	break;

		name = io_mgets(io, "=%s");

		if	((c = io_getc(io)) == '=')
		{
			p = io_mgets(io, "%s");
			arg = mpsubvec(p, 0, NULL);
			memfree(p);
		}
		else
		{
			io_ungetc(c, io);
			arg = NULL;
		}

		if	(mstrcmp(NAME, name) == 0)
		{
			memfree(BaseName);
			BaseName = arg;
			arg = NULL;
		}
		else if	((out = get_output(name)) != NULL)
		{
			open_output(out, arg, head);
		}
		else
		{
			dbg_note(NULL, "[1]", "s", name);
		}

		memfree(name);
		memfree(arg);
	}
	
	if	(MakeList || MakeRule)
		eval_stat = 0;

	divide_src();
	memfree(head);
	return NULL;
}

static STRBUF(rule_buf, 0);

static EfiObj *p_rule (IO *io, void *data)
{
	int c;
	char *p;

	c = io_eat(io, " \t");
	sb_putc('\n', &rule_buf);
	p = getstring(io);
	sb_puts(p, &rule_buf);
	memfree(p);

	if	(rule_buf.data[rule_buf.pos - 1] != '\n')
		sb_putc('\n', &rule_buf);

	return NULL;
}

static EfiParseDef pdef[] = {
	{ "config", p_config, NULL },
	{ "rule", p_rule, NULL },
	{ "rules", p_rule, NULL },
};

static void add_comment (const char *txt)
{
	if	(txt)
	{
		put_comment(txt, output[OUT_SRC].io);

		if	(hdr_comment)
			put_comment(txt, output[OUT_HDR].io);
	}
}

static void f_usecomment (EfiFunc *func, void *rval, void **arg)
{
	char *com = NULL;
	io_ctrl(Val_io(arg[0]), IOPP_COMMENT, &com);
	add_comment(com);
	Val_str(rval) = com;
}

static void f_addcomment (EfiFunc *func, void *rval, void **arg)
{
	add_comment(Val_str(arg[0]));
}

static void f_addflag (EfiFunc *func, void *rval, void **arg)
{
	sb_putc(' ', &sb_flags);
	sb_puts(Val_str(arg[0]), &sb_flags);
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_str, "usecomment (IO io = cin)", f_usecomment },
	{ 0, &Type_void, "addcomment (str com)", f_addcomment },
	{ 0, &Type_void, "addflag (str com)", f_addflag },
};


static void eval_file(const char *name)
{
	char *fname;
	IO *save_cin;

	fname = fsearch(IncPath, NULL, name, NULL);

	if	(fname == NULL)
		fname = fsearch(NULL, NULL, name, NULL);

	if	(fname == NULL)	return;

	if	(Verbose)
		io_xprintf(ioerr, "input < %s\n", fname);

	AddDepend(fname); 
	save_cin = CmdEval_cin;
	CmdEval_cin = io_fileopen(fname, "r");
	memfree(fname);
	CmdEval_cin = io_cmdpreproc(CmdEval_cin);

	while (eval_stat && io_eat(CmdEval_cin, ";%s") != EOF)
		UnrefEval(Parse_cmd(CmdEval_cin));

	io_close(CmdEval_cin);
	CmdEval_cin = save_cin;
}


int main (int narg, char **arg)
{
	int i;
	char *flags;

	ChangeLocale("C");
	SetVersion("$Id: mksource.c,v 1.44 2008-09-02 20:48:10 ef Exp $");
	SetProgName(arg[0]);
	bootstrap = mtabcat(" ", arg, narg);

	SetupStd();
	SetupUtil();
	SetupPreproc();

	SetupDataBase();
	SetupEDB();
	SetupTimeSeries();
	SetupRandom();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();
	SetupMathFunc();
	SetupStatData();
	SetupReadline();
	SetupDebug();

	AddParseDef(pdef, tabsize(pdef));
	AddFuncDef(fdef, tabsize(fdef));
	AddVarDef(NULL, globvar, tabsize(globvar));

	SetupOutput = 1;
	PushVarTab(NULL, NULL);
	AddVarDef(LocalVar, locvar, tabsize(locvar));
	ParseCommand(&narg, arg);
	PopVarTab();
	SetupOutput = 0;
	flags = sb_nul(&sb_flags);
	io_xprintf(ioerr, "flags=%#s\n", flags);

	if	(Verbose)
		io_xprintf(ioerr, "%s\n", bootstrap);

/*	Standardnamen bestimmen
*/
	if	(Template)
	{
		char *dname, *suffix;
		dname = mdirname(Template, 0);
		BaseName = mbasename(Template, &suffix);

		if	(dname)
		{
			char *p = IncPath;
			IncPath = mstrpaste(":", p, dname);
			memfree(p);
		}

		if	(suffix == NULL)
		{
			StrBuf *sb = sb_acquire();

			if	(dname)
			{
				sb_puts(dname, sb);
				sb_putc('/', sb);
			}

			sb_puts(BaseName, sb);
			sb_puts(".tpl", sb);

			memfree(Template);
			Template = sb_cpyrelease(sb);
		}
	}

	if	(MakeDep || MakeRule)	MakeDepend = 1;

	for (i = 0; i < tabsize(output); i++)
	{
		if	(output[i].flag)
			open_output(output + i, NULL, NULL);

		add_output(output + i);
	}

	divide_src();
	eval_file(Template);

	for (i = 0; i < tabsize(output); i++)
		close_output(output + i);

	if	(MakeList)
	{
		FILE *file = fileopen(ListName, "w");

		for (i = 0; i < tabsize(output); i++)
			if	(output[i].flag)
				fprintf(file, "%s\n", output[i].fname);

		fileclose(file);
	}

	if	(!MakeDep && !MakeRule)
		return 0;

/*	Abhängigkeitsziel bestimmen
*/
	if	(!DependName)
	{
		DependName = mtabcat(" ", TargetList.data, TargetList.used);
		
		if	(!DependName)	return 0;
	}

	if	(MakeRule && Template)
	{
		char *arg[2];

		arg[0] = bootstrap;
		arg[1] = BaseName;

		io_psubvec(iostd, R_HEAD, 2, arg);

		if	(AllTarget)
			printf("\n%s:: %s\n", AllTarget, DependName);

		if	(DependTarget)
			printf("\n%s:: %s\n\t%s%s -d %s >> $(MAKEFILE)\n",
				DependTarget, Template, ProgName, flags,
				Template);

		if	(CleanTarget)
		{
			printf("\n%s::\n\trm -f", CleanTarget);

			for (i = 0; i < tabsize(output); i++)
				if	(output[i].flag)
					printf(" %s", output[i].fname);

			printf("\n");
		}

		printf("\n%s: %s\n\t%s%s %s\n",
			DependName, Template, ProgName, flags, Template);

		if	(rule_buf.pos)
		{
			IO *in;

			AddVarDef(LocalVar, locvar, tabsize(locvar));

			in = io_cstr("#include <ms_rules.hdr>\n");
			in = io_cmdpreproc(in);
			CmdEval(in, iostd);
			io_close(in);

			sb_setpos(&rule_buf, 0);
			in = io_strbuf(&rule_buf);
			in = io_cmdpreproc(in);
			io_pcopy(in, iostd, EOF, 2, arg);
			io_close(in);
		}

		io_psubvec(iostd, R_END, 2, arg);
	}

	if	(MakeDep)
		MakeDependList(iostd, DependName);

	return 0;
}
