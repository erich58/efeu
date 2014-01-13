/*
:*:	pattern matching
:de:	Mustervergleich

$Header	<EFEU/$1>

$Copyright (C) 2007 Erich Frühstück
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

#ifndef	EFEU_MatchPar_h
#define	EFEU_MatchPar_h	1

#include <EFEU/patcmp.h>
#include <EFEU/refdata.h>
#include <EFEU/io.h>
#include <regex.h>

typedef struct MatchPar MatchPar;

struct MatchPar {
	REFVAR;
	MatchPar *next;	/* Für Verkettungen */
	char *pattern;	/* Musterkennung */
	int flag;	/* Negationsflag */
	size_t minval;	/* Minimalwert */
	size_t maxval;	/* Maximalwert */
	regex_t exp;	/* übersetzter Ausdruck */
	int (*cmp) (MatchPar *par, const char *s, size_t n);
};

MatchPar *MatchPar_create (const char *def, size_t dim);
MatchPar *MatchPar_list (const char *list, size_t dim);
MatchPar *MatchPar_vec (char **list, size_t ldim, size_t dim);
MatchPar *MatchPar_scan (IO *io, int delim, size_t dim);
int MatchPar_exec (MatchPar *match, const char *name, size_t idx);

#endif	/* EFEU/patcmp.h */
