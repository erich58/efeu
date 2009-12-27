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

#include <EFEU/KeyTab.h>
#include <EFEU/CmdPar.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <EFEU/Info.h>
#include <EFEU/appl.h>

static KEYTAB(EvalTab, 32);
static void setup_builtin (void);

/*
Die Funktion |$1| liefert die durch <name> definierte
Auswertungsfunktion für Befehlszeilenparameter.
*/

CmdParEval_t *CmdParEval_get (const char *name)
{
	setup_builtin();
	return StrKey_get(&EvalTab, name, NULL);
}

/*
Die Funktion |$1| erweitert die Auswertungsfunktionen für
Befehlszeilenparameter mit <def>.
*/

void CmdParEval_add (CmdParEval_t *eval)
{
	setup_builtin();
	StrKey_add(&EvalTab, eval);
}

/*
Die Funktion |$1| listet die Auswertungsfunktionen für
Befehlszeilenparameter auf.
*/

void CmdParEval_show (io_t *io, const char *fmt)
{
	CmdParEval_t **p;
	size_t n;

	setup_builtin();

	for (n = EvalTab.used, p = EvalTab.data; n-- > 0; p++)
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

static int f_test (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *p;

	p = CmdPar_psub(cpar, par, arg);
	io_printf(ioerr, "%s=%#s, ", var->name, var->value);
	io_printf(ioerr, "%#s, %#s -> %#s\n", par, arg, p);
	memfree(p);
	return 0;
}


static int f_message (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *p = CmdPar_psub(cpar, par, arg);
	io_puts(p, ioerr);
	memfree(p);
	return 0;
}

static int f_set (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *p = CmdPar_psub(cpar, par, arg);
	memfree(var->value);
	var->value = p;
	return 0;
}

static int f_lset (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *p = CmdPar_psub(cpar, par, arg);
	memfree(var->value);
	var->value = mlangcpy(p, NULL);
	memfree(p);
	return 0;
}

static int f_insert (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *x = var->value;
	var->value = mstrpaste(par, arg, x);
	memfree(x);
	return 0;
}


static int f_append (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *x = var->value;
	var->value = mstrpaste(par, x, arg);
	memfree(x);
	return 0;
}

static int f_usage (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *fmt = CmdPar_psub(cpar, par, arg);
	CmdPar_usage(cpar, NULL, fmt);
	memfree(fmt);
	return 0;
}

static int f_manpage (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	if	(arg && *arg == '@')
	{
		CmdPar_usage(cpar, iostd, arg);
	}
	else
	{
		char *name = CmdPar_psub(cpar, par, arg);
		io_t *io = io_fileopen(name, "w");
		memfree(name);
		CmdPar_manpage(cpar, io);
		io_close(io);
	}

	return 0;
}

static int f_config (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *name;
	io_t *io;

	name = CmdPar_psub(cpar, par, arg);

	if	((io = io_applfile(name, APPL_CNF)) != NULL)
	{
		CmdPar_read (cpar, io, EOF, 0);
		io_close(io);
	}
	else	message(NULL, MSG_EFM, 22, 1, name);

	memfree(name);
	return 0;
}

static int f_break (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	return CMDPAR_BREAK;
}

static int f_exit (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	exit(par ? atoi(par) : 0);
	return 0;
}

static int f_info (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *node = CmdPar_psub(cpar, par, arg);
	PrintInfo(iostd, NULL, node);
	memfree(node);
	return 0;
}

static int f_dump (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *node = CmdPar_psub(cpar, par, arg);
	DumpInfo(iostd, GetInfo(NULL, NULL), node);
	memfree(node);
	return 0;
}

static CmdParEval_t builtin[] = {
	 { "test", "Testauswertungsfunktion", f_test },
	 { "set", "Wert setzen", f_set },
	 { "lset", "Wert mit Sprachfilter setzen", f_lset },
	 { "message", "Meldung ausgeben", f_message },
	 { "insert", "Argument mit Trennzeichen einfügen", f_insert },
	 { "append", "Argument mit Trennzeichen anhängen", f_append },
	 { "config", "Konfigurationsdatei laden", f_config },
	 { "usage", "Aufrufverwendung generieren", f_usage },
	 { "manpage", "Handbucheintrag generieren", f_manpage },
	 { "break", "Optionsabfrage abbrechen", f_break },
	 { "exit", "Verarbeitung abbrechen", f_exit },
	 { "info", "Informationseintrag abrufen", f_info },
	 { "dump", "Informationsdatenbank ausgeben", f_dump },
};


static void setup_builtin(void)
{
	static int setup_done = 0;
	int i;

	if	(setup_done)	return;

	setup_done = 1;

	for (i = 0; i < tabsize(builtin); i++)
		StrKey_add(&EvalTab, builtin + i);
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

