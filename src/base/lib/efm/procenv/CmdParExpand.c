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

#include <EFEU/KeyTab.h>
#include <EFEU/CmdPar.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <EFEU/Info.h>
#include <EFEU/appl.h>

#define	FMT_HEAD	"\\dhead\t$1(1)\n\\mpage[1] $1\n"

static KEYTAB(ExpandTab, 32);
static void setup_builtin (void);

/*
Die Funktion |$1| liefert die durch <name> definierte
Expansionsfunktion für Beschreibungstexte.
*/

CmdParExpand_t *CmdParExpand_get (const char *name)
{
	setup_builtin();
	return StrKey_get(&ExpandTab, name, NULL);
}

/*
Die Funktion |$1| erweitert die Expansionsfunktionen für
Beschreibungstexte mit <def>.
*/

void CmdParExpand_add (CmdParExpand_t *eval)
{
	setup_builtin();
	StrKey_add(&ExpandTab, eval);
}

/*
Die Funktion |$1| listet die Expansionsfunktionen für
Beschreibungstexte auf.
*/

void CmdParExpand_show (io_t *io, const char *fmt)
{
	CmdParExpand_t **p;
	size_t n;

	setup_builtin();

	for (n = ExpandTab.used, p = ExpandTab.data; n-- > 0; p++)
	{
		if	(fmt)
		{
			reg_cpy(1, (*p)->name);
			reg_cpy(2, (*p)->desc);
			io_psub(io, fmt);
		}
		else	io_printf(io, "[%s] %#s\n", (*p)->name, (*p)->desc);
	}
}


/*	Eingebaute Funktionen
*/

static void f_head (CmdPar_t *par, io_t *io, const char *arg)
{
	reg_cpy(1, par->name);
	io_psub(io, FMT_HEAD);
}

static void f_ident (CmdPar_t *par, io_t *io, const char *arg)
{
	io_printf(io, "%s -- %s\n", par->name,
		CmdPar_getval(par, "Ident", par->name));
}

static void f_synopsis (CmdPar_t *par, io_t *io, const char *arg)
{
	CmdPar_synopsis(par, io);
}

static void f_arglist (CmdPar_t *par, io_t *io, const char *arg)
{
	CmdPar_arglist(par, io);
}

static void f_environ (CmdPar_t *par, io_t *io, const char *arg)
{
	CmdPar_environ(par, io);
}

static void f_varlist (CmdPar_t *par, io_t *io, const char *arg)
{
	CmdPar_showval(par, io, arg);
}

static CmdParExpand_t builtin[] = {
	{ "head",	"Handbuchkopf", f_head },
	{ "name",	"Handbuchkopf", f_head },
	{ "ident",	"Überschrift", f_ident },
	{ "synopsis",	"Übersicht", f_synopsis },
	{ "arglist",	"Programmparameter", f_arglist },
	{ "environ",	"Umgebungsvariablen", f_environ },
	{ "varlist",	"Variablenliste", f_varlist },
};


static void setup_builtin(void)
{
	static int setup_done = 0;
	int i;

	if	(setup_done)	return;

	setup_done = 1;

	for (i = 0; i < tabsize(builtin); i++)
		StrKey_add(&ExpandTab, builtin + i);
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

