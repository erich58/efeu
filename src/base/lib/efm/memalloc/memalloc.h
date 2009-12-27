/*
:*:memory administration
:de:Speicherplatzverwaltung

$Header	<EFEU/$1>

$Copyright (C) 1996 Erich Frühstück
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

#ifndef	EFEU_memalloc_h
#define	EFEU_memalloc_h	1

#include <EFEU/config.h>

#ifndef	sizealign

/*
:*:The macro |$1| alligns the size <x> in times of <y>.
:de:Der Makro |$1| dient zur Ausrichtung der Speichergröße <x> auf
ganzzahlige Vielfache von <y>.
*/

#define	sizealign(x, y)	((y) * (((x) + (y) - 1) / (y)))
#endif

#ifndef	tabsize

/*
:*:
The macro |$1| returns the first dimension of the field <x>.
It is used for fields, wich dimensions is only determined by the number
of initialisation values.
:de:
Der Makro |$1| liefert die erste Dimension des Datenfeldes <x>.  Er wird für
globale oder statische Felder verwendet, deren erste Dimension nicht
vorgegebnen ist, sondern aus der Zahl der Initialisierungswerte bestimmt  wird.
*/

#define	tabsize(x)	(sizeof(x) / sizeof(x[0]))
#endif

#ifndef	tabparm

/*
:*:
The macro |$1| returns pointer, number of elements and element size of
field. It could be used in the argumentlist of functions like 
|qsort| or |bsearch|.
:de:
Der Makro |$1| liefert den Pointer, die Zahl der Elemente und die
Elementgröße eines Datenfeldes. Er wird typischerweise bei der Übergabe
von Feldern an Funktionen wie |qsort| und |bsearch|.
*/

#define	tabparm(x)	(void *) (x), tabsize(x), sizeof(x[0])
#endif

void *lmalloc (size_t size);
void *lrealloc (void *ptr, size_t size);
void lfree (void *ptr);
void lcheck (void *ptr);
void lcheckall (void);

/*
:*:The type |$1| is used as allocation list node.
:de:Der Datentyp |$1| wird für den Knoten einer Zuweisungsliste verwendet.
*/

typedef struct AllocTabListStruct {
	struct AllocTabListStruct *next;
} AllocTabList;

/*
:*:
The structure |$1| is usesd to administrate
memmory pieces with fixed size |elsize|.
:de:
Die Datenstruktur |$1| definiert eine Speichersegmenttabelle zur
Verwaltung von Speichersegmenten der Größe |elsize|.

:de:
Sie enthält eine Liste der freien Elemente
(|freelist|) und eine Liste mit Datenblöcken (|blklist|).
Die Zahl der freien Segmente wird in |nfree| und die Zahl der
benutzten Segmente wird in |nused| gespeichert.
Mit den Funktionen |new_data| und |del_data| können Datensegmente
entnommen und wieder hineiungestellt werden. Vergleiche dazu
\mref{alloctab(3)}.

:de:
Zum Auffüllen der freien Liste wird ein Block mit |blksize| Elementen
angefordert. Jedem Block ist eine Kettenstruktur vorangestellt (Für
die Liste der Datenblöcke).  Der Speicherbedarf für einen Block
beträgt |sizeof(AllocTabList)| + |blksize| * |elsize|.
Datenblöcke, die zum Füllen der freien Liste angefordert wurden,
werden nicht mehr freigegeben.

:de:
Die Datensegmentgröße muß auf die Größe einer Kettenstruktur
ausgerichtet sein.  Freie Elemente werden zu einer Liste verkettet und
können dort entnommen werden.
*/

typedef struct {
	size_t blksize;
	size_t elsize;
	size_t nfree;
	size_t nused;
	AllocTabList *blklist;
	AllocTabList *freelist;
} AllocTab;

void *new_data (AllocTab *tab);
int del_data (AllocTab *tab, void *data);
int tst_data (AllocTab *tab, void *data);
void check_data (AllocTab *tab);

#define	ALLOCSIZE(size)		sizealign(size, sizeof(AllocTabList))

/*
:de:
Der Makro |$1| liefert die Initialisierungswerte für eine Zuweisungstabelle
zur Verwaltung von Speicherelementen fixer Länge.
Die Länge eines Segmentes wird durch <size> festgelegt.
Durch <blk> wird die Zahl der Elemente eines Blockes festgelegt.
Falls <blk> den Wert 0 hat, werden Standardvorgaben verwendet.
*/

#define	ALLOCDATA(blk,size)	{ blk, ALLOCSIZE(size), 0, 0, NULL, NULL, }

/*
:de:
Der Makro |$1| initialisiert eine Zuweisungstabelle
zur Verwaltung von Speicherelementen fixer Länge unter dem Namen <name>.
Die Länge eines Segmentes wird durch <size> festgelegt.
Durch <blk> wird die Zahl der Elemente eines Blockes festgelegt.
Falls <blk> den Wert 0 hat, werden Standardvorgaben verwendet.
*/

#define	ALLOCTAB(name,blk,size)	AllocTab name = ALLOCDATA(blk, size)


void *memalloc (size_t size);
void memnotice (void *ptr);
void memfree (void *ptr);

void memstat (const char *prompt);
extern int memtrace;
void meminfo (const char *prompt);
void memchange (const char *prompt);

void memcheck (void);
void memswap (void *ap, void *bp, size_t n);

#endif	/* EFEU/memalloc.h */
