/*
Prozessumgebung

$Copyright (C) 2001 Erich Fr�hst�ck
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
#include <EFEU/LangType.h>

/*	Systemabh�ngige Parameter:

S_PGM_SHELL	Standardbefehlsinterpreter
S_PGM_PAGER	Standardseitenfilter
*/

#define S_PGM_SHELL	"/bin/sh"
#define S_PGM_PAGER	"|less -r"

/*
Die Variable |$1| enth�lt den Basisnamen des Kommandos.
*/

char *ProgName = NULL;

/*
Die Variable |$1| enth�lt die Kommandoidentifikation.
Sie unterscheidet sich im normalfall nicht von |ProgName|.
*/

char *ProgIdent = NULL;

/*
Die Variable |$1| enth�lt den Namen der Shell f�r Unterprozesse.
*/

char *Shell = S_PGM_SHELL;

/*
Die Variable |$1| enth�lt den Namen des Kommandos f�r eine
Seitenorientierte ausgabe.
*/

char *Pager = S_PGM_PAGER;

static int c_name (io_t *in, io_t *out, void *arg)
{
	return io_puts(ProgName, out);
}

static int c_id (io_t *in, io_t *out, void *arg)
{
	return io_puts(ProgIdent, out);
}

/*
static int c_font(io_t *in, io_t *out, void *arg)
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
	fname_t *pgname;

	if	(need_setup)
	{
		SetLangType(NULL);
		psubfunc('!', c_name, NULL);
		psubfunc('%', c_id, NULL);
		/*
		psubfunc('R', c_font, (void *) IO_ROMAN_FONT);
		psubfunc('B', c_font, (void *) IO_BOLD_FONT);
		psubfunc('I', c_font, (void *) IO_ITALIC_FONT);
		*/
		need_setup = 0;
	}

	if	((pgname = strtofn(name)) != NULL)
	{
		CmdPar_t *par = CmdPar_ptr(NULL);

		memfree(ProgName);
		memfree(ProgIdent);
		ProgName = mstrcpy(pgname->name);
		ProgIdent = mstrcpy(ProgName);
		SetApplPath(pgname->path);
		memfree(pgname);
		memfree(par->name);
		par->name = mstrcpy(ProgName);

		if	(need_config)
		{
			CmdPar_load(par, "efm");
			need_config = 0;
		}
	}
	else	SetApplPath(NULL);

	SetInfoPath(NULL);
}

/*
$Notes
Die Prozessumgebung wird �berarbeitet und daher kann sich an den
einzelnen Variablen einiges �ndern.  Dies sollte keine Kompatiblit�tsprobleme
aufwerfen, da die Variablen nur Bibliotheksintern verwendung finden.

$SeeAlso
\mref{getenv(3)} im @PRM.
*/
