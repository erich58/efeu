/*
:*:	regular expressions
:de:	Reguläre Ausdrücke

$Header	<EFEU/$1>

$Copyright (C) 2000 Erich Frühstück
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

#ifndef	_EFEU_RegExp_h
#define	_EFEU_RegExp_h	1

#include <EFEU/refdata.h>
#include <regex.h>

/*
:de:
Die Datenstruktur |$1| dient zur Verwaltung von regulären Ausdrücken.
Für Debuggingzwecke wird nicht nur der übersetzte Ausdruck, sondern
auch der Definitionsstring in der Struktur gespeichert.
*/

typedef struct {
	REFVAR;		/* Referenzvariablen */
	int icase;	/* Flag zur Fallunterscheidung */
	char *def;	/* Definitionsstring */
	regex_t exp;	/* übersetzter Ausdruck */
} RegExp_t;

/*
:de:
Der Makro |$1| legt die Maximalzahl von Teilmustern fest, die
mit |RegExp_exec| abgefragt werden können.
*/

#define	REGEXP_MAXSUB	32

extern reftype_t RegExp_reftype;
extern char *RegExp_error;

RegExp_t *RegExp (const char *str, int flags);
int RegExp_exec (RegExp_t *exp, const char *str, regmatch_t **ptr);
char *RegExp_subst (RegExp_t *exp, const char *repl,
	const char *str, int glob);

#endif	/* EFEU/RegExp.h */
