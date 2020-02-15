/*
:*:	command parameters
:de:	Kommandoparameter

$Header	<EFEU/$1>

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

#ifndef	EFEU_CmdPar_h
#define	EFEU_CmdPar_h	1

#include <EFEU/io.h>
#include <EFEU/vecbuf.h>
#include <EFEU/Info.h>
#include <EFEU/ArgList.h>

typedef struct CmdParStruct CmdPar;
typedef struct CmdParKeyStruct CmdParKey;
typedef struct CmdParVarStruct CmdParVar;
typedef struct CmdParEvalStruct CmdParEval;
typedef struct CmdParCallStruct CmdParCall;
typedef struct CmdParDefStruct CmdParDef;
typedef struct CmdParExpandStruct CmdParExpand;

/*
de:
Die Datenstruktur |$1| dient zur Konfiguration der Parameter
für ein ausführbares Programm. Mithilfe dieser Datenstruktur
werden Umgebungsvariablen und Befehlszeilenparameter abgefragt.
Weiters erlaubt sie die Ausgabe der Kommandosyntax.
*/

struct CmdParStruct {
	REFVAR;		/* Referenzvariablen */
	char *name;	/* Aufrufname */
	VecBuf var;	/* Variablentabelle */
	VecBuf def;	/* Definitionstabellen */
	VecBuf env;	/* Umgebungsvariablen */
	VecBuf opt;	/* Optionen */
	VecBuf arg;	/* Argumente */
};

extern RefType CmdPar_reftype;
void CmdPar_init (CmdPar *par);
void CmdPar_clean (CmdPar *par);
void CmdPar_info (CmdPar *par, InfoNode *node);
CmdPar *CmdPar_alloc (const char *name);
CmdPar *CmdPar_ptr (CmdPar *ptr);

/*
:de:
Die Datenstruktur |$1| definiert Variablen zur Abfrage von
Programmparametern. Die Variablen werden in der Tabelle <var>
der Kommandoparameterstruktur gespeichert.
*/

struct CmdParVarStruct {
	char *name;	/* Variablenname */
	char *value;	/* Variablenwerte */
	char *desc;	/* Beschreibung */
};

CmdParVar *CmdPar_var (CmdPar *par, const char *name, int flag);
void CmdPar_setval (CmdPar *par, const char *name, char *value);
char *CmdPar_getval (CmdPar *par, const char *name, const char *defval);
void CmdPar_showval (CmdPar *par, IO *io, const char *fmt);

/**/

#define	CMDPAR_ERR	0x1	/* Fehler bei der Abfrage */
#define	CMDPAR_BREAK	0x2	/* Abbruch der Verarbeitung */
#define	CMDPAR_KEEP	0x4	/* Abgefragte Argumente erhalten */

#define	CMDPAR_END	(CMDPAR_ERR|CMDPAR_BREAK)

/*
:de:
Die Datenstruktur |$1| definiert Auswertungsfunktionen zur
Verarbeitung von Parameterdefinitionen.
*/

struct CmdParEvalStruct {
	char *name;	/* Name der Funktion */
	char *desc;	/* Beschreibungstext */
	int (*func) (CmdPar *cmdpar, CmdParVar *var, const char *par,
		const char *arg);
};

CmdParEval *CmdParEval_get (const char *name);
void CmdParEval_add (CmdParEval *eval);
void CmdParEval_show (IO *out, const char *fmt);

/*
:de:
Die Datenstruktur |$1| definiert eine Aufrufstruktur zur Auswertung.
*/

struct CmdParCallStruct {
	CmdParCall *next;	/* Nächster Aufruf */
	CmdParEval *eval;	/* Auswertungsdefinition */
	char *name;		/* Resourcename */
	char *par;		/* Auswertungsparameter */
};

CmdParCall *CmdParCall_alloc (void);
void CmdParCall_free (CmdParCall *call);
void CmdParCall_print (IO *out, CmdParCall *call);
int CmdParCall_eval (CmdPar *par, CmdParCall *def, const char *arg);

/**/

#define	PARTYPE_ENV	1	/* Umgebungsvariable */
#define	PARTYPE_OPT	2	/* Option */
#define	PARTYPE_ARG	3	/* Argument */

#define	ARGTYPE_NONE	0	/* Kein Argument */
#define	ARGTYPE_STD	1	/* Standardargument */
#define	ARGTYPE_OPT	2	/* Optionales Argument */
#define	ARGTYPE_LAST	3	/* Letztes Argument */
#define	ARGTYPE_VA0	4	/* Variable Argumentliste >= 0 */
#define	ARGTYPE_VA1	5	/* Variable Argumentliste >= 1 */
#define	ARGTYPE_REGEX	6	/* Regulärer Ausdruck */
#define	ARGTYPE_VALUE	7	/* Zuweisungswert */

/*
:de:
Die Struktur |$1| definiert einen Parameterschlüssel.
*/

struct CmdParKeyStruct {
	short partype;		/* Parametertype */
	short argtype;		/* Argumenttype */
	char *key;		/* Kennung */
	char *val; 		/* Vorgabewert */
	char *arg; 		/* Argumentname */
	CmdParDef *def;		/* Parameterdefinition */
	CmdParKey *next;	/* Nächster Schlüssel */
};

CmdParKey *CmdParKey_alloc (void);
void CmdParKey_free (CmdParKey *def);
void CmdParKey_print (IO *out, CmdParKey *key);

/*
:de:
Die Struktur |$1| definiert eine Parameterdefinition.
*/

struct CmdParDefStruct {
	CmdParKey *key;		/* Schlüsselliste */
	CmdParCall *call;	/* Aufrufliste */
	char *desc;		/* Beschreibungstext */
};

CmdParDef *CmdParDef_alloc (void);
void CmdParDef_free (CmdParDef *def);
void CmdParDef_print (IO *out, CmdParDef *def);
int CmdParDef_eval (CmdPar *par, CmdParDef *def, const char *arg);


/*	Standardroutinen
*/

void CmdPar_add (CmdPar *par, CmdParDef *def);
void CmdPar_read (CmdPar *par, IO *in, int end, int flag);
void CmdPar_write (CmdPar *par, IO *out);
int CmdPar_load (CmdPar *par, const char *name, int flag);
int CmdPar_eval (CmdPar *par, int *narg, char **arg, int flag);

char *CmdPar_psub (CmdPar *par, const char *fmt, const char *arg);
void CmdPar_psubout (CmdPar *par, IO *out, const char *fmt, ArgList *args);

/*
:de:
Die Datenstruktur |$1| definiert Auswertungsfunktionen zur
Expansion von Beschreibungstexten.
*/

struct CmdParExpandStruct {
	char *name;	/* Name der Funktion */
	char *desc;	/* Beschreibungstext */
	void (*eval) (CmdPar *par, IO *out, const char *arg);
};

CmdParExpand *CmdParExpand_get (const char *name);
void CmdParExpand_add (CmdParExpand *eval);
void CmdParExpand_show (IO *io, const char *fmt);


/*	Verwendungsinformationen
*/

extern int CmdPar_docmode;
void CmdPar_synopsis (CmdPar *par, IO *out, int flag);
void CmdPar_arglist (CmdPar *par, IO *out);
void CmdPar_options (CmdPar *par, IO *out);
void CmdPar_environ (CmdPar *par, IO *out);
void CmdPar_resource (CmdPar *par, IO *out);
void CmdPar_expand (CmdPar *par, IO *in, IO *out);
void CmdPar_iousage (CmdPar *par, IO *out, IO *def);
void CmdPar_usage (CmdPar *par, IO *out, const char *fmt);
void CmdPar_manpage (CmdPar *par, IO *out);
char *CmdPar_hpath (CmdPar *par);

/*
$SeeAlso
\mref{CmdPar(3)},
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
\mref{CmdPar_write(3)}.
*/

#endif	/* EFEU/CmdPar.h */
