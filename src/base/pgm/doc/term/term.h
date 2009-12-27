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

#ifndef	_EFEU_term_h
#define	_EFEU_term_h	1

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
} TermPar_t;

#define	TERM_INDENT	4	/* Einrücktiefe */

extern TermPar_t TermPar;
void TermPar_init(void);

typedef struct term_s term_t;
typedef struct TermVar_s TermVar_t;

struct TermVar_s {
	TermVar_t *next;
	unsigned margin;
	void (*caption) (term_t *trm, int flag);
};

struct term_s {
	DOCDRV_VAR;		/* Standardausgabevariablen */
	unsigned col;		/* Aktuelle Spalte */
	unsigned space;		/* Leerzeichen ausgeben */
	unsigned mode;		/* Leerzeichenmodus */
	unsigned hangpar;	/* Hängende Absätze */
	unsigned hang;		/* Einrücktiefe nach Umbruch */
	TermVar_t var;		/* Umgebungsvariablen */
	stack_t *s_att;		/* Stack mit Attributen */
	char *att;		/* Aktuelles Attribut */
};

extern io_t *DocOut_term (io_t *io);

extern void term_push (term_t *trm);
extern void term_pop (term_t *trm);

extern void term_att (term_t *trm, int flag, char *att);
extern int term_putc (term_t *mr, int c);
extern int term_verb (term_t *mr, int c);
extern void term_string (term_t *mr, const char *str);
extern void term_newline (term_t *mr, int flag);
extern void term_hmode (term_t *mr);
extern int term_cmd (term_t *mr, va_list list);
extern int term_env (term_t *mr, int flag, va_list list);

#endif	/* EFEU/term.h */
