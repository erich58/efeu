/*
Programmkonfiguration

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_pconfig_H
#define	EFEU_pconfig_H	1

#include <EFEU/Info.h>
#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/procenv.h>
#include <EFEU/appl.h>

/*	Programmparameter
*/

typedef struct {
	int flag;	/* Steuerflag */
	char *key;	/* Schlüsselwort */
	char *name;	/* Argumentname */
	char *cmd;	/* Rückruffunktion */
	char *desc;	/* Beschreibungsformat */
} pardef_t;

#define	P_INIT		1	/* Initialisierungsstring */
#define	P_ENV		2	/* Environmentparameter */
#define	P_OPT		3	/* Optionen */
#define	P_XOPT		4	/* Spezialoptionen */
#define	P_ARG		5	/* Argumente */
#define	P_XARG		6	/* Spezialargumente */
#define	P_OPTARG	7	/* Optionale Argumente */
#define	P_BREAK		8	/* Abbruch der Argumentanalyse */
#define	P_SET		9	/* Variable initialisieren */
#define	P_REGEX		10	/* Regulärer Ausdruck */


/*	Globale Programmparameter
*/

typedef struct {
	char *init;	/* Konfigurationsdatei */
	vecbuf_t env;	/* Environmentstack */
	vecbuf_t opt;	/* Optionsstack */
	vecbuf_t arg;	/* Argumentstack */
	vecbuf_t xopt;	/* Spezialoptionen */
	vecbuf_t xarg;	/* Spezialargumente */
} deftab_t;

extern deftab_t PgmDefTab;

/*	Initialisierungsfunktionen
*/

void SetupReadline (void);
void SetupDebug (void);

/*	Programmkonfiguration
*/

typedef struct {
	char *name;	/* Programmname */
	int (*eval) (int argc, char **argv);
} pconfig_t;

/*	Initialisierungsfunktionen
*/

void libinit (const char *name);
void pconfig_init (void);
void pconfig_exit (void);
void pconfig (const char *name, Var_t *var, size_t dim);
void loadarg (int *narg, char **arg);
void usage (const char *fmt, io_t *out);
void cp_usage(io_t *in, io_t *out);

#define	PROCINFONAME	"self"

extern InfoNode_t *ProcInfo;
void ArgInfo (io_t *io, InfoNode_t *info);

int applfile (const char *name, int type);

/*	Globale Variablen
*/

extern char *ProgIdent;
extern char *ProgName;
extern char *ApplPath;
extern char *HelpFmt;
extern char *UsageFmt;

extern char *Shell;
extern char *Pager;
extern char *PS1;
extern char *PS2;

extern int float_align;

/*	Systemaufrufe
*/

int callproc (const char *cmd);

extern xtab_t Buildin;
extern int ExecFlag;
extern int TraceFlag;

/*	Hilfsprogramme zur Argumentzerlegung
*/

double sepfactor (const char *p, char **ptr);
char *sepname (const char *p, char **ptr);
char *ExpandPath (const char *name);

/*	Interpreterprogramme
*/

int EshConfig (int *narg, char **arg);
void EshIdent (io_t *in);

#endif	/* EFEU/pconfig.h */
