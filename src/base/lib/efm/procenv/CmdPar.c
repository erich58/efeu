/*
:*:administation of command parameters
:de:Verwalten von Kommandoparameterstrukturen

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

#include <EFEU/CmdPar.h>
#include <EFEU/vecbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/Info.h>

#define LBL_RES	":*:resources" \
		":de:Kommandoresourcen"

#define LBL_ARG	":*:options and arguments" \
		":de:Optionen und Argumente"

#define	LBL_ENV	":*:environment" \
		":de:Umgebungsvariablen"

static char *cpar_ident (const void *data)
{
	const CmdPar *par = data;
	return mstrcpy(par->name);
}

static void cpar_clean (void *ptr)
{
	CmdPar_clean(ptr);
	memfree(ptr);
}

/*
Die Datenstruktur |$1| definiert den Referenztype für die Struktur |CmdPar|.
*/

RefType CmdPar_reftype = REFTYPE_INIT("CmdPar", cpar_ident, cpar_clean);


/*	Hilfsfunktionen
*/

/**/
static void var_clean (void *data)
{
	CmdParVar *var = data;
	memfree(var->name);
	memfree(var->value);
	memfree(var->desc);
}

/**/
static void def_clean (void *data)
{
	CmdParDef **def = data;
	CmdParDef_free(*def);
}

/*
Die Funktion |$1| initialisiert die Kommandoparameterstruktur <par>.
*/

void CmdPar_init (CmdPar *par)
{
	if	(!par)	return;

	par->name = NULL;
	vb_init(&par->var, 32, sizeof(CmdParVar));
	vb_init(&par->def, 32, sizeof(CmdParDef *));
	vb_init(&par->env, 32, sizeof(CmdParKey *));
	vb_init(&par->opt, 32, sizeof(CmdParKey *));
	vb_init(&par->arg, 32, sizeof(CmdParKey *));
}

/*
Die Funktion |$1| löscht alle in der Kommandoparameterstruktur <par>
gespeicherten Daten und stellt den Zustand nach |CmdPar_init| her.
*/

void CmdPar_clean (CmdPar *par)
{
	if	(!par)	return;

	memfree(par->name);
	par->name = NULL;
	vb_clean(&par->var, var_clean);
	vb_free(&par->var);
	vb_clean(&par->def, def_clean);
	vb_free(&par->def);
	vb_free(&par->env);
	vb_free(&par->opt);
	vb_free(&par->arg);
}

/*
Die Funktion |$1| liefert den Pointer auf eine dynamisch generierte und
initialisierte Kommandoparameterstruktur.
*/

CmdPar *CmdPar_alloc (const char *name)
{
	CmdPar *par = memalloc(sizeof(CmdPar));
	CmdPar_init(par);
	par->name = mstrcpy(name);
	return rd_init(&CmdPar_reftype, par);
}

/*
Die Funktion |$1| wird intern zur Bestimmung eines Vorgabewertes
für den Pointer <ptr> auf eine Kommandostruktur verwendet.
Falls für <ptr> ein Nullpointer übergeben wird, liefert die
die Funktion |$1| den Pointer auf eine interne Datenstruktur zurück.
Diese wird bei der ersten Verwendung automatisch initialisiert.
Ansonsten liefert die Funktion <ptr>.
*/

CmdPar *CmdPar_ptr (CmdPar *ptr)
{
	static CmdPar *defpar = NULL;

	if	(ptr)	return ptr;

	if	(!defpar)
		defpar = CmdPar_alloc(NULL);

	return defpar;
}

#define	WELCOME	"{} - {Ident}\n{Copyright}\n"

static void arg_info (IO *io, InfoNode *info)
{
	int save = CmdPar_docmode;
	CmdPar_docmode = 0;
	CmdPar_arglist(info->par, io);
	CmdPar_docmode = save;
}

static void env_info (IO *io, InfoNode *info)
{
	int save = CmdPar_docmode;
	CmdPar_docmode = 0;
	CmdPar_environ(info->par, io);
	CmdPar_docmode = save;
}

static void var_info (IO *io, InfoNode *info)
{
	int save = CmdPar_docmode;
	CmdPar_docmode = 0;
	CmdPar_resource(info->par, io);
	CmdPar_docmode = save;
}

/*
Die Funktion |$1| generiert aus der Kommandoparameterstruktur die folgenden
Unterpunkte für den Informationsknoten <node>:
[Res]	Kommandoresourcen
[Arg]	Optionen und Argumente
[Env]	Umgebungsvariablen
*/

void CmdPar_info (CmdPar *par, InfoNode *node)
{
	par = CmdPar_ptr(NULL);
	node = GetInfo(node, NULL);
	node->label = CmdPar_getval(par, "Ident", NULL);
	node->func = NULL;
	node->par = NULL;
	/*
	node->par = CmdPar_psub(par, WELCOME, par->name);
	*/
	AddInfo(node, "Res", LBL_RES, var_info, rd_refer(par));
	AddInfo(node, "Arg", LBL_ARG, arg_info, rd_refer(par));
	AddInfo(node, "Env", LBL_ENV, env_info, rd_refer(par));
}

/*
$Note
Die hier beschriebenen Funktionen werden von einem Kommando nur
dann benötigt, wenn es mit mehr als einer Kommandoparameterstruktur
arbeitet. Alle anderen Funktionen zur Verwaltung der Kommandoparameter
kännen mit einen Nullpointer anstelle einer Kommandoparameterstruktur
aufgerufen werden, da sie mithilfe von |CmdPar_ptr| auf
eine interne Variable zurückgreifen.

$SeeAlso
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
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

