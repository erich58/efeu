/*
Dokumentgenerator

$Copyright (C) 1999 Erich Fr�hst�ck
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
#include <EFEU/Readline.h>
#include <EFEU/calendar.h>
#include <EFEU/LangDef.h>
#include "efeudoc.h"
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <EFEU/object.h>
#include <EFEU/pconfig.h>
#include <EFEU/MakeDepend.h>
#include "LaTeX.h"
#include "mroff.h"

#define	CFNAME	"efeudoc"
#define	HNAME	"~/.doc_history"
#define	DHEAD	"This file was generated by $!"
#define	FHEAD	"document input: $0"


/*	Globale Variablen
*/

char *DocPath = NULL;
char *DocName = NULL;
char *DocInit = NULL;
char *DocExit = NULL;
char *DocStyle = NULL;

EfiVarDef globvar[] = {
	{ "DocPath", &Type_str, &DocPath },
	{ "DocName", &Type_str, &DocName },
	{ "DocInit", &Type_str, &DocInit },
	{ "DocExit", &Type_str, &DocExit },
	{ "DocStyle", &Type_str, &DocStyle },
};


static void eval_file (Doc *doc, const char *path, const char *name)
{
	IO *in;
	char *p;
	char *id;

	in = name ? Doc_open(path, name, 1) : io_interact(NULL, HNAME);
	id = rd_ident(in);
	p = mpsubvec(FHEAD, 1, &id);
	memfree(id);
	io_ctrl(doc->out, DOC_REM, p);
	memfree(p);
	Doc_copy(doc, in);
	io_close(in);
}


int main (int narg, char **arg)
{
	Doc *doc;
	char *Type;
	char *Output;
	char *p;
	int i;

	SetVersion("$Id: efeudoc.c,v 1.28 2003-01-06 09:06:14 ef Exp $");
	SetProgName(arg[0]);
	SetupDoc();
	SetupReadline();
	SetupDebug();
	AddVarDef(NULL, globvar, tabsize(globvar));
	DocTab_fload(GlobalDocTab, CFNAME);
	DocMacInfo("DocMac", ":*:document macros:de:Makrodefinitionen");
	DocOutInfo("DocOut", ":*:document drivers:de:Dokumentausgabefilter");
	LaTeX_setup();
	mroff_setup();
	ParseCommand(&narg, arg);
	MakeDepend = GetFlagResource("MakeDepend");

#if	0
	if	(getuid() == 0)
	{
		dbg_note(NULL, "[Doc:0]", NULL);
		return 1;
	}
#endif

	DocTab_fload(GlobalDocTab, GetResource("Config", NULL));
	Type = GetResource("Type", NULL);
	Output = GetResource("Output", NULL);
	doc = Doc_create(Type);
	doc->stat = GetFlagResource("NoHead");
	doc->out = DocOut(doc->type, Output);
	Doc_eval(doc, NULL, DocInit);

	if	((p = GetResource("Head", NULL)) != NULL)
	{
		IO *in = io_cstr(p);
		Doc_copy(doc, in);
		io_close(in);
	}

	p = mpsubvec(DHEAD, 0, NULL);
	io_ctrl(doc->out, DOC_REM, p);
	memfree(p);

	for (i = 1; i < narg; i++)
	{
		char *p = argval(arg[i]);
		eval_file(doc, CFGPATH, p);
		memfree(p);
	}

	Doc_eval(doc, NULL, DocExit);
	Doc_endall(doc, 0);
	rd_deref(doc);

	if	(!MakeDepend)	return 0;
	if	(!MakeDepend)	return 0;
	
	p = GetResource("TaskName", NULL);

	if	(p && Output)
	{
		StrBuf *sb = new_strbuf(0);

		if	(LangDef.langdef)
			sb_printf(sb, "LANG=%#s ", LangDef.langdef);

		sb_puts(arg[0], sb);
		sb_puts(GetResource("Bootstrap", NULL), sb);

		if	(Type)
			sb_printf(sb, " -T %#s", Type);

		sb_printf(sb, " -o %#s", Output);

		for (i = 1; i < narg; i++)
		{
			sb_putc(' ', sb);
			sb_puts(arg[i], sb);
		}

		sb_putc(0, sb);
		MakeTaskRule(iostd, p);
		MakeTargetRule(iostd, (char *) sb->data);
		del_strbuf(sb);
		return 0;
	}

	MakeDependRule(iostd);
	return 0;
}
