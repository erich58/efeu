/*
:*:	basic types for EFEU-libraries
:de:	Basistypen f�r EFEU-Programmbibliothek

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Fr�hst�ck
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

#ifndef	_EFEU_types_h
#define	_EFEU_types_h	1

#include <EFEU/config.h>

/*
:de:
Die folgenden Datentypen werden in cast-Ausdr�cken bei der �bergabe von
Funktionspointern verwendet (siehe Beispiel weiter unten).

[|$1|]
	:*:compare funktion for pointer;
	:de:allgemeine Vergleichsfunktion f�r Datenpointer;

$Example
:de:
Der Datentype |comp_t| definiert eine Vergleichsfunktion, wie
sie von \mref{bsearch(3C)}, \mref{qsort(3C)} oder \mref{vbsearch(3)}
als Argument ben�tigt wird.
\par
Anstelle der Definition einer Vergleichsfunktion mit |void *| Pointern,
wird direkt der ben�tigte Datentype verwendet und daf�r bei der
�bergabe der Funktion der cast |(comp_t)| verwendet.
\par
Die folgende Tabelle
---- verbatim
char *tab[] = {
	"Birnen", "Weintrauben", "Orangen", "Bananen"
};
----
wird mit
---- verbatim
qsort(tab, sizeof(tab)/sizeof(tab[0]), sizeof(tab[0]), (comp_t) strcmp);
----
alphabetisch sortiert.
*/

typedef int (*comp_t) (const void *a, const void *b);

/*
[|$1|]	
	:*:test function for pointer;
	:de:allgemeine Testfunktion f�r Datenpointer;
*/

typedef int (*test_t) (const void *a);

/*
[|$1|]
	:*:visit function for lists;
	:de:Funktion zum Durchwandern von Datenlisten;
*/

typedef int (*visit_t) (void *ptr);

/*
[|$1|]
	:*:initialisation function;
	:de:Initialisierungsfunktion;
*/

typedef void (*setup_t) (void);

/*
[|$1|]
	:*:identification function;
	:de:Identifikationskennung;
*/

typedef char *(*ident_t) (const void *data);

/*
[|$1|]
	:*:allocation function;
	:de:Datenobjekt einrichten;
*/

typedef void *(*create_t) (void);

/*
[|$1|]
	:*:deallocation function;
	:de:Datenobjekt freigeben;
*/

typedef void (*clean_t) (void *ptr);

/*
[|$1|]
	:*:copy function;
	:de:Datenobjekt kopieren;
*/

typedef void *(*copy_t) (const void *src);


/*
[|$1|]
	:*:administration function, integrates
	:de:Datenobjekt verwalten, integriert
	:_:|create_t|, |copy_t| und |clean_t|;
*/

typedef void *(*admin_t) (void *tg, const void *src);

/*
:*:chain structrure |$1|
:de:Kettenstruktur |$1|
*/

typedef struct chain_s {
	struct chain_s *next;	/* Verweis auf Nachfolger */
} chain_t;

/*
:*:label |$1|
:de:Label |$1|
*/

typedef struct {
	char *name;
	char *desc;
} Label_t;

#ifndef	sizealign

/*
:*:The macro |$1| alligns the size <x> in times of <y>.
:de:Der Makro |$1| dient zur Ausrichtung der Speichergr��e <x> auf
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
Der Makro |$1| liefert die erste Dimension des Datenfeldes <x>.  Er wird f�r
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
Elementgr��e eines Datenfeldes. Er wird typischerweise bei der �bergabe
von Feldern an Funktionen wie |qsort| und |bsearch|.
*/

#define	tabparm(x)	(void *) (x), tabsize(x), sizeof(x[0])
#endif

/*
$SeeAlso
\mref{vbsearch(3)}, \mref{config(5)}, \mref{admin(5)}.\br
\mref{bsearch(3C)}, \mref{qsort(3C)} im @PRM.
*/

#endif	/* EFEU/types.h */
