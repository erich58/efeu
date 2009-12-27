/*
Verwalten Kommandoparameterstrukturen

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
#include <EFEU/KeyTab.h>
#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/Info.h>

#define LBL_RES	":*:resources" \
		":de:Kommandoresourcen"

#define LBL_ARG	":*:options and arguments" \
		":de:Optionen und Argumente"

#define	LBL_ENV	":*:environment" \
		":de:Umgebungsvariablen"

static char *cpar_ident (CmdPar_t *par)
{
	return par ? mstrcpy(par->name) : NULL;
}

static CmdPar_t *cpar_admin (CmdPar_t *tg, const CmdPar_t *src)
{
	if	(tg)
	{
		CmdPar_clean(tg);

		if	(src == NULL)
		{
			memfree(tg);
			tg = NULL;
		}
	}
	else	tg = src ? src : memalloc(sizeof(CmdPar_t));

	CmdPar_init(tg);
	return tg;
}

/*
Die Datenstruktur |$1| definiert den Referenztype für die Struktur |CmdPar_t|.
*/
ADMINREFTYPE(CmdPar_reftype, "CmdPar", cpar_ident, cpar_admin);


/*	Hilfsfunktionen
*/

/**/
static void var_clean (CmdParVar_t *var)
{
	memfree(var->name);
	memfree(var->value);
	memfree(var->desc);
}

/**/
static void def_clean (CmdParDef_t **def)
{
	CmdParDef_free(*def);
}

/*
Die Funktion |$1| initialisiert die Kommandoparameterstruktur <par>.
*/

void CmdPar_init (CmdPar_t *par)
{
	if	(!par)	return;

	par->name = NULL;
	vb_init(&par->var, 32, sizeof(CmdParVar_t));
	vb_init(&par->def, 32, sizeof(CmdParDef_t *));
	vb_init(&par->env, 32, sizeof(CmdParKey_t *));
	vb_init(&par->opt, 32, sizeof(CmdParKey_t *));
	vb_init(&par->arg, 32, sizeof(CmdParKey_t *));
}

/*
Die Funktion |$1| löscht alle in der Kommandoparameterstruktur <par>
gespeicherten Daten und stellt den Zustand nach |CmdPar_init| her.
*/

void CmdPar_clean (CmdPar_t *par)
{
	if	(!par)	return;

	memfree(par->name);
	par->name = NULL;
	vb_clean(&par->var, (clean_t) var_clean);
	vb_free(&par->var);
	vb_clean(&par->def, (clean_t) def_clean);
	vb_free(&par->def);
	vb_free(&par->env);
	vb_free(&par->opt);
	vb_free(&par->arg);
}

/*
Die Funktion |$1| liefert den Pointer auf eine dynamisch generierte und
initialisierte Kommandoparameterstruktur.
*/

CmdPar_t *CmdPar_alloc (const char *name)
{
	CmdPar_t *par = rd_create(&CmdPar_reftype);
	par->name = mstrcpy(name);
	return par;
}

/*
Die Funktion |$1| wird intern zur Bestimmung eines Vorgabewertes
für den Pointer <ptr> auf eine Kommandostruktur verwendet.
Falls für <ptr> ein Nullpointer übergeben wird, liefert die
die Funktion |$1| den Pointer auf eine interne Datenstruktur zurück.
Diese wird bei der ersten Verwendung automatisch initialisiert.
Ansonsten liefert die Funktion <ptr>.
*/

CmdPar_t *CmdPar_ptr (CmdPar_t *ptr)
{
	static CmdPar_t *defpar = NULL;

	if	(ptr)	return ptr;

	if	(!defpar)
		defpar = rd_create(&CmdPar_reftype);

	return defpar;
}

#define	WELCOME	"{} - {Ident}\n{Copyright}\n"

static void arg_info (io_t *io, InfoNode_t *info)
{
	int save = CmdPar_docmode;
	CmdPar_docmode = 0;
	CmdPar_arglist(info->par, io);
	CmdPar_docmode = save;
}

static void env_info (io_t *io, InfoNode_t *info)
{
	int save = CmdPar_docmode;
	CmdPar_docmode = 0;
	CmdPar_environ(info->par, io);
	CmdPar_docmode = save;
}

static void var_info (io_t *io, InfoNode_t *info)
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

void CmdPar_info (CmdPar_t *par, InfoNode_t *node)
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

