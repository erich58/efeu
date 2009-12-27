/*
Große Datenmengen sortieren

$Header	<EFEU/$1>

$Copyright (C) 1995 Erich Frühstück
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

#ifndef EFEU_bigsort_h
#define EFEU_bigsort_h	1

#include <EFEU/ftools.h>
#include <EFEU/vecbuf.h>

#define	MSG_BIGSORT	"bigsort"

#define	BS_MAXMERGE	4	/* Maximazahl der mischbaren Dateien */

extern int BigSortDebug;


/*	Sortierstruktur
*/

typedef struct {
	size_t size;
	void *data;
} BigSortPtr_t;

typedef struct {
	size_t recl;	/* Satzlänge  */
	size_t size;	/* Maximale Buffergröße */
	size_t pos;	/* Position im Buffer */
	size_t dim;	/* Maximale Zahl der Sätze */
	size_t idx;	/* Aktueller Satzindex */
	char *data;	/* Zwischenspeicher */
	comp_t comp;	/* Vergleichsfunktion */
	vecbuf_t ftab;	/* Tabelle mit Zwischenfiles */
	BigSortPtr_t *ptr;	/* Pointervektor */
} BigSort_t;


extern BigSort_t *BigSort (size_t recl, size_t dim, size_t size, comp_t comp);
extern void BigSortReorg (BigSort_t *sort);
extern void BigSortClose (BigSort_t *sort);
extern void *BigSortNext (BigSort_t *sort, size_t recl);
extern void *BigSortData (BigSort_t *sort, size_t *size);

#endif	/* EFEU/bigsort.h */
