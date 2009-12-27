/*
Speicherplatzverwaltung

$Header	<EFEU/$1>

$Copyright (C) 1996 Erich Fr�hst�ck
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

#ifndef	_EFEU_memalloc_h
#define	_EFEU_memalloc_h	1

#include <EFEU/config.h>
#include <EFEU/types.h>


/*	Elementare Speicherzuweisung
*/

void *lmalloc (size_t size);
void lfree (void *ptr);
void lcheck (void *ptr);


/*
Die Datenstruktur |$1| definiert eine Speichersegmenttabelle zur
Verwaltung von Speichersegmenten der Gr��e |elsize|.

Sie enth�lt eine Liste der freien Elemente
(|freelist|) und eine Liste mit Datenbl�cken (|blklist|).
Die Zahl der freien Segmente wird in |nfree| und die Zahl der
benutzten Segmente wird in |nused| gespeichert.
Mit den Funktionen |new_data| und |del_data| k�nnen Datensegmente
entnommen und wieder hineiungestellt werden. Vergleiche dazu
\mref{alloctab(3)}.

Zum Auff�llen der freien Liste wird ein Block mit |blksize| Elementen
angefordert. Jedem Block ist eine Kettenstruktur vorangestellt (F�r
die Liste der Datenbl�cke).  Der Speicherbedarf f�r einen Block
betr�gt |sizeof(chain_t)| + |blksize| * |elsize|.
Datenbl�cke, die zum F�llen der freien Liste angefordert wurden,
werden nicht mehr freigegeben.

Die Datensegmentgr��e mu� auf die Gr��e einer Kettenstruktur
ausgerichtet sein.  Freie Elemente werden zu einer Liste verkettet und
k�nnen dort entnommen werden.
*/

typedef struct {
	size_t blksize;
	size_t elsize;
	size_t nfree;
	size_t nused;
	chain_t *blklist;
	chain_t *freelist;
} alloctab_t;

void *new_data (alloctab_t *tab);
int del_data (alloctab_t *tab, void *data);
int tst_data (alloctab_t *tab, void *data);
void check_data (alloctab_t *tab);
void *admin_data (alloctab_t *tab, void *tg, const void *src);

#define	ALLOCSIZE(size)		sizealign(size, sizeof(chain_t))

/*
Der Makro |$1| liefert die Initialisierungswerte f�r eine Zuweisungstabelle
zur Verwaltung von Speicherelementen fixer L�nge.
Die L�nge eines Segmentes wird durch <size> festgelegt.
Durch <blk> wird die Zahl der Elemente eines Blockes festgelegt.
Falls <blk> den Wert 0 hat, werden Standardvorgaben verwendet.
*/

#define	ALLOCDATA(blk,size)	{ blk, ALLOCSIZE(size), 0, 0, NULL, NULL, }

/*
Der Makro |$1| initialisiert eine Zuweisungstabelle
zur Verwaltung von Speicherelementen fixer L�nge unter dem Namen <name>.
Die L�nge eines Segmentes wird durch <size> festgelegt.
Durch <blk> wird die Zahl der Elemente eines Blockes festgelegt.
Falls <blk> den Wert 0 hat, werden Standardvorgaben verwendet.
*/

#define	ALLOCTAB(name,blk,size)	alloctab_t name = ALLOCDATA(blk, size)


/*	Robuste Speicherzuweisung
*/

void *memalloc (size_t size);
void memnotice (void *ptr);
void memfree (void *ptr);
void memstat (const char *prompt);
void memcheck (void);
void memclean (void **ptr);
void *memadmin (void *tg, const void *src, size_t size);
void memswap (void *ap, void *bp, size_t n);

#endif	/* EFEU/memalloc.h */
