/*
Selektion von Achsennamen

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

#ifndef	EFEU_mdtest_h
#define	EFEU_mdtest_h	1

#include <EFEU/mdmat.h>

typedef struct mdtest_struct mdtest;

struct mdtest_struct {
	mdtest *next;	/* Für Verkettungen */
	char *pattern;	/* Musterkennung */
	int flag;	/* Negationsflag */
	size_t minval;	/* Minimalwert */
	size_t maxval;	/* Maximalwert */
	int (*cmp) (mdtest *t, const char *s, size_t n);
};

mdtest *mdtest_create (const char *def, size_t dim);
void mdtest_clean (mdtest *test);
mdtest *mdmktestlist (const char *list, size_t dim);
mdtest *mdtestlist (char **list, size_t ldim, size_t dim);
int mdtest_eval (mdtest *test, const char *name, size_t idx);

#endif	/* EFEU/mdmat.h */
