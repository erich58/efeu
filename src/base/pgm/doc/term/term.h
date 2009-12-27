/*
term-Ausgabefilter

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	term_h
#define	term_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include <DocDrv.h>

typedef struct {
	int wpmargin;	/* Umbruchspalte */
	char *rm;	/* Wechsel zu Roman */
	char *it;	/* Wechsel zu Italic */
	char *tt;	/* Wechsel zu Schreibmascinenschrift */
	char *bf;	/* Wechsel zu Fettschrift */
	char *Name;	/* Name im Handbucheintrag */
	char *TabName;	/* Name in der Tabellenüberschrift */
	char *FigName;	/* Name in der Abbildungsüberschrift */
} TermPar;

#define	TERM_INDENT	4	/* Einrücktiefe */

extern TermPar term_par;
void TermPar_init(void);

typedef struct TermStruct Term;
typedef struct TermVarStruct TermVar;

struct TermVarStruct {
	TermVar *next;
	unsigned margin;
	void (*caption) (Term *trm, int flag);
};

struct TermStruct {
	DOCDRV_VAR;		/* Standardausgabevariablen */
	unsigned col;		/* Aktuelle Spalte */
	unsigned space;		/* Leerzeichen ausgeben */
	unsigned mode;		/* Leerzeichenmodus */
	unsigned hangpar;	/* Hängende Absätze */
	unsigned hang;		/* Einrücktiefe nach Umbruch */
	TermVar var;		/* Umgebungsvariablen */
	Stack *s_att;		/* Stack mit Attributen */
	char *att;		/* Aktuelles Attribut */
};

extern IO *DocOut_term (IO *io);

extern void term_push (Term *trm);
extern void term_pop (Term *trm);

extern void term_att (Term *trm, int flag, char *att);
extern void term_string (Term *mr, const char *str);
extern void term_newline (Term *mr, int flag);
extern void term_hmode (Term *mr);

extern int term_verb (void *drv, int c);
extern int term_putc (void *drv, int c);
extern int term_cmd (void *drv, va_list list);
extern int term_env (void *drv, int flag, va_list list);

#endif	/* term.h */
