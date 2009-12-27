/*
Dokumentgenerator

$Copyright (C) 1999 Erich Frühstück
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
#include "efeudoc.h"
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <EFEU/object.h>
#include <EFEU/pconfig.h>
#include <EFEU/MakeDepend.h>
#include "LaTeX.h"

#define	CFNAME	"efeudoc"
#define	HNAME	"~/.doc_history"
#define	HEAD	"Dokumenteingabe: $0"


/*	Globale Variablen
*/

char *DocPath = NULL;
char *DocName = NULL;
char *DocInit = NULL;
char *DocExit = NULL;
char *DocStyle = NULL;

VarDef_t globvar[] = {
	{ "DocPath", &Type_str, &DocPath },
	{ "DocName", &Type_str, &DocName },
	{ "DocInit", &Type_str, &DocInit },
	{ "DocExit", &Type_str, &DocExit },
	{ "DocStyle", &Type_str, &DocStyle },
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
	Doc_t *doc;
	char *Type;
	char *Output;
	char *p;
	int i;

	SetProgName(arg[0]);
	SetupDoc();
	SetupReadline();
	SetupDebug();
	AddVarDef(NULL, globvar, tabsize(globvar));
	ParseCommand(&narg, arg);
	MakeDepend = GetFlagResource("MakeDepend");
	LaTeX_setup();
	DocTab_fload(GlobalDocTab, CFNAME);
	DocOutInfo("DocOut", "Dokumentausgabefilter");
	DocMacInfo("DocMac", "Makrodefinitionen");

#if	0
	if	(getuid() == 0)
	{
		message(NULL, MSG_DOC, 0, 0);
		return 1;
	}
#endif

	DocTab_fload(GlobalDocTab, GetResource("Config", NULL));
	Type = GetResource("Type", NULL);
	Output = GetResource("Output", NULL);
	doc = Doc(Type);
	doc->stat = GetFlagResource("NoHead");
	doc->out = DocOut(doc->type, Output);
	Doc_eval(doc, NULL, DocInit);

	if	((p = GetResource("Head", NULL)) != NULL)
	{
		io_t *in = io_cstr(p);
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

	if	(!MakeDepend)	return 0;
	if	(!MakeDepend)	return 0;
	
	p = GetResource("TaskName", NULL);

	if	(p && Output)
	{
		strbuf_t *sb = new_strbuf(0);

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
