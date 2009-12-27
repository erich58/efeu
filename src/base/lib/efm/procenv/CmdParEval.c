/*
Auswertungsfunktionen für Kommandoparameter abfragen/definieren

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

static NameKeyTab EvalTab = NKT_DATA("CmdParEval", 32, NULL);
static void setup_builtin (void);

/*
Die Funktion |$1| liefert die durch <name> definierte
Auswertungsfunktion für Befehlszeilenparameter.
*/

CmdParEval *CmdParEval_get (const char *name)
{
	setup_builtin();
	return nkt_fetch(&EvalTab, name, NULL);
}

/*
Die Funktion |$1| erweitert die Auswertungsfunktionen für
Befehlszeilenparameter mit <def>.
*/

void CmdParEval_add (CmdParEval *eval)
{
	setup_builtin();
	nkt_insert(&EvalTab, eval->name, eval);
}

static int show_std (const char *name, void *data, void *ptr)
{
	CmdParEval *eval = data;
	io_xprintf(ptr, "[%s] %#s\n", name, eval->desc);
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
	arg_cset(par->arg, 2, eval->desc);
	CmdPar_psubout(par->cpar, par->out, par->fmt, par->arg);
	return 0;
}

/*
Die Funktion |$1| listet die Auswertungsfunktionen für
Befehlszeilenparameter auf.
*/

void CmdParEval_show (IO *io, const char *fmt)
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
		nkt_walk(&EvalTab, show_fmt, &par);
		rd_deref(par.arg);
	}
	else	nkt_walk(&EvalTab, show_std, io);
}


/*	Eingebaute Funktionen
*/

static int f_test (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *p;

	p = CmdPar_psub(cpar, par, arg);
	io_xprintf(ioerr, "%s=%#s, ", var->name, var->value);
	io_xprintf(ioerr, "%#s, %#s -> %#s\n", par, arg, p);
	memfree(p);
	return 0;
}


static int f_message (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *p = CmdPar_psub(cpar, par, arg);
	io_puts(p, ioerr);
	memfree(p);
	return 0;
}

static int f_set (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *p = par ? CmdPar_psub(cpar, par, arg) : NULL;
	memfree(var->value);
	var->value = p;
	return 0;
}

static int f_vset (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *fmt = CmdPar_getval(cpar, par, NULL);
	char *p = fmt ? CmdPar_psub(cpar, fmt, arg) : NULL;
	memfree(var->value);
	var->value = p;
	return 0;
}

static int f_lset (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *p = CmdPar_psub(cpar, par, arg);
	memfree(var->value);
	var->value = mlangcpy(p, NULL);
	memfree(p);
	return 0;
}

static int f_insert (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *x = var->value;
	var->value = mstrpaste(par, arg, x);
	memfree(x);
	return 0;
}


static int f_append (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *x = var->value;
	var->value = mstrpaste(par, x, arg);
	memfree(x);
	return 0;
}

static int f_usage (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *fmt = CmdPar_psub(cpar, par, arg);
	CmdPar_usage(cpar, NULL, fmt);
	memfree(fmt);
	return 0;
}

static int f_manpage (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	if	(arg && *arg == '@')
	{
		CmdPar_usage(cpar, iostd, arg);
	}
	else
	{
		char *name = CmdPar_psub(cpar, par, arg);
		IO *io = io_fileopen(name, "w");
		memfree(name);
		CmdPar_manpage(cpar, io);
		io_close(io);
	}

	return 0;
}

static int f_config (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *name;
	IO *io;

	name = CmdPar_psub(cpar, par, arg);

	if	((io = io_applfile(name, APPL_CNF)) != NULL)
	{
		CmdPar_read (cpar, io, EOF, 0);
		io_close(io);
		memfree(name);
	}
	else	log_note(NULL, "[efm:22]", "m", name);

	return 0;
}

static int f_break (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	return CMDPAR_BREAK;
}

static int f_exit (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	exit(par ? atoi(par) : 0);
	return 0;
}

static int f_info (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *node = CmdPar_psub(cpar, par, arg);
	BrowseInfo(node);
	memfree(node);
	return 0;
}

static CmdParEval builtin[] = {
	 { "test", "Testauswertungsfunktion", f_test },
	 { "set", "Wert setzen", f_set },
	 { "lset", "Wert mit Sprachfilter setzen", f_lset },
	 { "vset", "Wert mit Variable als Format setzen", f_vset },
	 { "message", "Meldung ausgeben", f_message },
	 { "insert", "Argument mit Trennzeichen einfügen", f_insert },
	 { "append", "Argument mit Trennzeichen anhängen", f_append },
	 { "config", "Konfigurationsdatei laden", f_config },
	 { "usage", "Aufrufverwendung generieren", f_usage },
	 { "manpage", "Handbucheintrag generieren", f_manpage },
	 { "break", "Optionsabfrage abbrechen", f_break },
	 { "exit", "Verarbeitung abbrechen", f_exit },
	 { "info", "Informationseintrag abrufen", f_info },
};


static void setup_builtin(void)
{
	static int setup_done = 0;
	int i;

	if	(setup_done)	return;

	setup_done = 1;

	for (i = 0; i < tabsize(builtin); i++)
		nkt_insert(&EvalTab, builtin[i].name, builtin + i);
}


/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParExpand(3)},
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

