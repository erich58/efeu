/*
Dokumenteinbindung

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

#include "efeudoc.h"
#include <EFEU/cmdeval.h>
#include <EFEU/preproc.h>
#include <ctype.h>
#include <sys/stat.h>

#define	MAINDOC	"main.eds"


static int testkey (const char *key, const char **ptr)
{
	const char *p;

	for (p = *ptr; *key != 0; p++, key++)
		if	(*p != *key)	return 0;

	if	(isalpha(*p))	return 0;

	*ptr = p;
	return 1;
}


#define	set_type(type)	if (base) alt = type; else base = type

void Doc_input (Doc *doc, const char *opt, IO *in)
{
	char *cmd = NULL;
	char *tmp = NULL;
	int eval = 0;
	int cfg = 0;
	int code = 0;
	int base = 0;
	int alt = 0;

	if	(!opt)
	{
		Doc_copy(doc, in);
		io_close(in);
		return;
	}

/*	Optionen bestimmen
*/
	while (*opt != 0)
	{
		if	(*opt == '|')
		{
			do	opt++;
			while	(*opt == ' ' || *opt == '\t');

			cmd = *opt ? mstrcpy(opt) : NULL;
			break;
		}

		if	(isspace(*opt) || *opt == ',')
			opt++;
		else if	(testkey("verbatim", &opt))
			base = alt, alt = DOC_MODE_VERB;
		else if	(testkey("ignore", &opt))
			base = alt, alt = DOC_MODE_SKIP;
		else if	(testkey("html", &opt))
			base = alt, alt = DOC_MODE_HTML;
		else if	(testkey("latex", &opt))
			base = alt, alt = DOC_MODE_TEX;
		else if	(testkey("man", &opt))
			base = alt, alt = DOC_MODE_MAN;
		else if	(testkey("code", &opt))
			code = 1;
		else if	(testkey("config", &opt))
			cfg = 1;
		else if	(testkey("eval", &opt))
			eval = 1;
		else if	(testkey("geval", &opt))
			eval = 2;
		else
		{
			dbg_note(NULL, "[Doc:11]", "s", opt);
			break;
		}
	}

	if	(alt && !base)
	{
		base = alt;
		alt = DOC_MODE_VERB;
	}

	Doc_rem(doc, "cmd=\"$1\" eval=$2 cfg=$3 base=$4 alt=$5", "nsddss",
		cmd, eval, cfg, DocEnvName(base), DocEnvName(alt));

/*	Eingabefilter
*/
	if	(cmd || eval)
	{
		IO *buf;

		tmp = newtemp(NULL, NULL);
		buf = io_fileopen(tmp, "w");

		if	(eval)
		{
			Doc_pushvar(doc);

			if	(eval > 1)
				PushVarTab(RefVarTab(GlobalVar), NULL);

			in = Doc_preproc(in);
			CmdEvalFunc(in, buf, 0);

			if	(eval > 1)
				PopVarTab();

			Doc_popvar(doc);
		}
		else	io_copy(in, buf);

		io_close(in);
		io_close(buf);

		if	(cmd)
		{
			char *p = mstrpaste(" < ", cmd, tmp);
			in = io_popen(p, "r");
			memfree(p);
			memfree(cmd);
		}
		else	in = io_fileopen(tmp, "r");
	}

/*	Verarbeiten
*/
	if	(cfg)
	{
		DocTab_load(Doc_gettab(doc, NULL), io_refer(in));
	}
	else if	(base)
	{
		Doc_verb(doc, in, base, alt);
	}
	else if	(code)
	{
		Doc_endall(doc, 0);
		Doc_newenv(doc, 0, DOC_ENV_CODE, NULL);
		Doc_copy(doc, in);
		Doc_endenv(doc);
	}
	else	Doc_copy(doc, in);

/*	Aufräumen
*/
	io_close(in);
	Doc_rem(doc, "EOF", NULL);

	if	(tmp)
		deltemp(tmp);
}

IO *Doc_open (const char *path, const char *name, int flag)
{
	IO *in;
	char *dname;
	char *ddir;
	
	dname = fsearch(path, NULL, name, NULL);
	ddir = NULL;

	if	(!dname)
		dname = fsearch(NULL, NULL, name, NULL);

	if	(dname)
	{
		struct stat buf;

		if	(strcmp(dname, "./.") == 0)
		{
			memfree(dname);
			dname = mstrcpy(".");
		}

		if	(stat(dname, &buf) == EOF)
		{
			dbg_note(NULL, "[Doc:4]", "m", dname);
			return NULL;
		}

		if	(S_ISDIR(buf.st_mode))
		{
			ddir = dname;
			dname = fsearch(ddir, NULL, "main", "eds");

			if	(!dname)
				dname = fsearch(ddir, NULL, "main", "doc");

			if	(!dname)
			{
				dname = mstrpaste("/", ddir, MAINDOC);
				dbg_note(NULL, "[Doc:5]", "m", dname);
				memfree(ddir);
				return NULL;
			}
		}
		else	ddir = mdirname(dname, 0);
	}
	else
	{
		dbg_note(NULL, "[Doc:5]", "s", name);
		return NULL;
	}

	in = io_lnum(io_fileopen(dname, "rzd"));

	memfree(DocName);
	DocName = dname;
	memfree(DocDir);
	DocDir = ddir ? ddir : mstrcpy(".");
	memfree(DocPath);
	
	if	(ddir)
	{
		int n = strlen(ddir);

		if	(strncmp(ddir, path, n) == 0 && path[n] == ':')
		{
			DocPath = mstrcpy(path);
		}
		else	DocPath = mstrpaste(":", ddir, path);
	}
	else	DocPath = mstrcpy(path);

	if	(flag)
	{
		char *fname, *suffix;

		fname = mbasename(DocName, &suffix);

		if	(suffix && isdigit(*suffix))
		{
			io_push(in, io_mstr(msprintf("\\mpage[%s] %s\n",
				suffix, fname)));
		}

		memfree(fname);
	}

	return in;
}

void Doc_include (Doc *doc, const char *opt, const char *name)
{
	IO *io;
	char *save_name;
	char *save_path;
	char *save_dir;
	
	save_name = DocName;
	save_path = DocPath;
	save_dir = DocDir;
	DocName = NULL;
	DocPath = NULL;
	DocDir = NULL;
	io = Doc_open(save_path, name, opt ? 0 : 1);
	Doc_rem(doc, "Eingabedatei: $1", "nm", rd_ident(io));
	Doc_input(doc, opt, io);
	memfree(DocName);
	memfree(DocPath);
	memfree(DocDir);
	DocName = save_name;
	DocPath = save_path;
	DocDir = save_dir;
}
