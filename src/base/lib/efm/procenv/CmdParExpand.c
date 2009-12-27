/*
Expansionsfunktionen für Beschreibungstexte abfragen/definieren

$Copyright (C) 2001 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/nkt.h>
#include <EFEU/CmdPar.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <EFEU/Info.h>
#include <EFEU/appl.h>
#include <ctype.h>

static NameKeyTab ExpandTab = NKT_DATA("Expand", 32, NULL);
static void setup_builtin (void);

/*
:*:The function |$1| returns a pointer to the expansion definition <name>
or a NULL pointer if not defined.
:de:Die Funktion |$1| liefert die durch <name> definierte
Expansionsfunktion für Beschreibungstexte.
*/

CmdParExpand *CmdParExpand_get (const char *name)
{
	setup_builtin();
	return nkt_fetch(&ExpandTab, name, NULL);
}

/*
:*:The function |$1| enhances the list of expansion definition
with <eval>.
:de:Die Funktion |$1| erweitert die Expansionsfunktionen für
Beschreibungstexte mit <def>.
*/

void CmdParExpand_add (CmdParExpand *eval)
{
	setup_builtin();
	nkt_insert(&ExpandTab, eval->name, eval);
}

static int show_std (const char *name, void *data, void *ptr)
{
	CmdParExpand *eval = data;
	char *desc = mlangcpy(eval->desc, NULL);
	io_printf(ptr, "[%s] %#s\n", name, desc);
	memfree(desc);
	return 0;
}

typedef struct {
	IO *out;
	const char *fmt;
	ArgList *arg;
	CmdPar *cpar;
} ShowPar;

static int show_fmt (const char *name, void *data, void *ptr)
{
	CmdParEval *eval = data;
	ShowPar *par = ptr;
	arg_cset(par->arg, 1, name);
	arg_mset(par->arg, 2, mlangcpy(eval->desc, NULL));
	CmdPar_psubout(par->cpar, par->out, par->fmt, par->arg);
	return 0;
}

/*
:*:The function |$1| writes a list of expansion definations to
the output <io>.
:de:Die Funktion |$1| listet die Expansionsfunktionen für
Beschreibungstexte auf.
*/

void CmdParExpand_show (IO *io, const char *fmt)
{
	setup_builtin();

	if	(fmt)
	{
		ShowPar par;
		par.out = io;
		par.fmt = fmt;
		par.arg = arg_create();
		par.cpar = CmdPar_ptr(NULL);
		arg_cset(par.arg, 0, NULL);
		nkt_walk(&ExpandTab, show_fmt, &par);
		rd_deref(par.arg);
	}
	else	nkt_walk(&ExpandTab, show_std, io);
}

/*	auxiliary functions
*/

static int testflag (const char *arg, int key)
{
	int flag;

	if	(arg == NULL)	return 0;

	for (flag = 0; *arg != 0; arg++)
	{
		if	(*arg == '-')	flag = 1;
		else if	(isspace(*arg))	flag = 0;
		else if (*arg == key)	return 1;
	}

	return 0;
}

static void putvar (CmdPar *par, IO *io, const char *name)
{
	CmdPar_psubout(par, io, CmdPar_getval(par, name, NULL), NULL);
}

/*	Builtin functions
*/

static void f_head (CmdPar *par, IO *io, const char *arg)
{
	io_psubarg(io, CmdPar_getval(par, ".manpage.head", NULL),
		"ns", par->name);
}

static void f_ident (CmdPar *par, IO *io, const char *arg)
{
	io_puts(par->name, io);
	io_puts(" -- ", io);
	CmdPar_psubout(par, io, CmdPar_getval(par, "Ident", par->name), NULL);
	io_putc('\n', io);
}

static void f_name (CmdPar *par, IO *io, const char *arg)
{
	f_head(par, io, arg);
	putvar(par, io, ".manpage.name");
	f_ident(par, io, arg);
}

static void f_synopsis (CmdPar *par, IO *io, const char *arg)
{
	if	(testflag(arg, 'h'))
		putvar(par, io, ".synopsis.head");

	if	(testflag(arg, 'i'))
		putvar(par, io, ".synopsis.intro");

	CmdPar_synopsis(par, io, testflag(arg, 'x'));
}

static void f_arglist (CmdPar *par, IO *io, const char *arg)
{
	if	(testflag(arg, 'h'))
		putvar(par, io, ".arglist.head");

	if	(testflag(arg, 'i'))
		putvar(par, io, ".arglist.intro");

	CmdPar_arglist(par, io);
}

static void f_options (CmdPar *par, IO *io, const char *arg)
{
	CmdPar_options(par, io);
}

static void f_environ (CmdPar *par, IO *io, const char *arg)
{
	if	(testflag(arg, 'h'))
		putvar(par, io, ".environ.head");

	CmdPar_environ(par, io);
}

static void f_version (CmdPar *par, IO *io, const char *arg)
{
	char *p = CmdPar_getval(par, "Version", NULL);

	if	(p == NULL)	return;

	if	(testflag(arg, 'h'))
		putvar(par, io, ".version.head");

	io_puts(p, io);
	io_putc('\n', io);
}

static void f_copyright (CmdPar *par, IO *io, const char *arg)
{
	char *p = CmdPar_getval(par, "Copyright", NULL);

	if	(p == NULL)	return;

	if	(testflag(arg, 'h'))
		putvar(par, io, ".copyright.head");

	io_puts(p, io);
	io_putc('\n', io);
}

static void f_varlist (CmdPar *par, IO *io, const char *arg)
{
	CmdPar_showval(par, io, arg);
}

static CmdParExpand builtin[] = {
	{ "head", ":*:manpage head:de:Handbuchkopf", f_head },
	{ "ident", ":*:caption:de:Überschrift", f_ident },
	{ "name", ":*:manpage head with caption"
		":de:Handbuchkopf mit Überschrift", f_name },
	{ "synopsis", ":*:Synopsis:de:Übersicht", f_synopsis },
	{ "arglist", ":*:command parameters:de:Programmparameter", f_arglist },
	{ "options", ":*:command options:de:Programmoptionen", f_options },
	{ "environ", ":*:Environment:de:Umgebungsvariablen", f_environ },
	{ "varlist", ":*:List of variables:de:Variablenliste", f_varlist },
	{ "version", ":*:version number:de:Versionsnummer", f_version },
	{ "copyright", ":*:Copyright information:de:Copyright", f_copyright },
};


static void setup_builtin(void)
{
	static int setup_done = 0;
	int i;

	if	(setup_done)	return;

	setup_done = 1;

	for (i = 0; i < tabsize(builtin); i++)
		nkt_insert(&ExpandTab, builtin[i].name, builtin + i);
}


/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
\mref{CmdParKey(3)},
\mref{CmdParVar(3)},
\mref{CmdPar_eval(3)},
\mref{CmdPar_list(3)},
\mref{CmdPar_load(3)},
\mref{CmdPar_psub(3)},
\mref{CmdPar_usage(3)},
\mref{CmdPar_write(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/

