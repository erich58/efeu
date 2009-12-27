/*
Prozessumgebung

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

#include <EFEU/procenv.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <EFEU/CmdPar.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <EFEU/LangDef.h>
#include <EFEU/locale.h>

/*	Systemabhängige Parameter:

S_PGM_SHELL	Standardbefehlsinterpreter
S_PGM_PAGER	Standardseitenfilter
*/

#define S_PGM_SHELL	"/bin/sh"
#define S_PGM_PAGER	"|less -r"

/*
Die Variable |$1| enthält den Verzeichnisnamen (falls bekannt) des Programms.
*/

char *ProgDir = NULL;

/*
Die Variable |$1| enthält den Basisnamen des Kommandos.
*/

char *ProgName = NULL;

/*
Die Variable |$1| enthält die Kommandoidentifikation.
Sie unterscheidet sich im Normalfall nicht von |ProgName|.
*/

char *ProgIdent = NULL;

/*
Die Variable |$1| enthält den Namen der Shell für Unterprozesse.
*/

char *Shell = S_PGM_SHELL;

/*
Die Variable |$1| enthält den Namen des Kommandos für eine
Seitenorientierte ausgabe.
*/

char *Pager = S_PGM_PAGER;

static int c_name (IO *in, IO *out, void *arg)
{
	return io_puts(ProgName, out);
}

static int c_id (IO *in, IO *out, void *arg)
{
	return io_puts(ProgIdent, out);
}

/*
static int c_font(IO *in, IO *out, void *arg)
{
	int n;

	n = io_ctrl(out, (int) (size_t) arg);
	return (n == EOF ? 0 : n);
}
*/

/*
Die Funktion |$1| initialisiert die globalen Variablen
|ProgName| und |ProgIdent| entsprechend dem Aufrufnamen
des Kommandos. Weiters wird |SetApplPath| mit dem Verzeichnisnamen
aufgerufen.
*/

void SetProgName (const char *name)
{
	static int need_setup = 1;
	static int need_config = 1;

	if	(need_setup)
	{
		ChangeLocale(NULL);
		SetLangDef(NULL);
		psubfunc('!', c_name, NULL);
		psubfunc('%', c_id, NULL);
		need_setup = 0;
	}

	if	(name && *name)
	{
		CmdPar *par;
		
		memfree(ProgDir);
		ProgDir = mdirname(name, 0);
		SetApplPath(ProgDir);

		par = CmdPar_ptr(NULL);
		memfree(par->name);
		par->name = mbasename(name, NULL);

		memfree(ProgName);
		memfree(ProgIdent);
		ProgName = mstrcpy(par->name);
		ProgIdent = mstrcpy(par->name);

		if	(need_config)
		{
			CmdPar_load(par, "efm", 0);
			need_config = 0;
		}
	}
	else	SetApplPath(NULL);

	SetInfoPath(NULL);
}

/*
$Notes
Die Prozessumgebung wird überarbeitet und daher kann sich an den
einzelnen Variablen einiges ändern.  Dies sollte keine Kompatiblitätsprobleme
aufwerfen, da die Variablen nur Bibliotheksintern verwendung finden.

$SeeAlso
\mref{getenv(3)} @PRM.
*/
