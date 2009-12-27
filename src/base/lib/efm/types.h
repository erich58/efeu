/*	Basistypen für EFEU-Programmbibliothek
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

$Header <EFEU/$1>

$SeeAlso
\mref{vbsearch(3)}, \mref{config(5)}, \mref{admin(5)}.\br
\mref{bsearch(3C)}, \mref{qsort(3C)} im @PRM.
*/

#ifndef	EFEU_TYPES_H
#define	EFEU_TYPES_H	1

#include <EFEU/config.h>

/*
Die folgenden Datentypen werden in cast-Ausdrücken bei der Übergabe von
Funktionspointern verwendet (siehe Beispiel weiter unten).

[|$1|]	Vergleichsfunktion zum Sortieren und Suchen

$Example
Der Datentype |comp_t| definiert eine Vergleichsfunktion, wie
sie von \mref{bsearch(3C)}, \mref{qsort(3C)} oder \mref{vbsearch(3)}
als Argument benötigt wird.

Anstelle der Definition einer Vergleichsfunktion mit |void *| Pointern,
wird direkt der benötigte Datentype verwendet und dafür bei der
Übergabe der Funktion der cast |(comp_t)| verwendet.

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

/*	[|$1|]	allgemeine Testfunktion für einen Datenpointer
*/

typedef int (*test_t) (const void *a);

/*	[|$1|]	Funktion zum Durchwandern von Datenlisten.
*/

typedef int (*visit_t) (void *ptr);

/*	[|$1|]	Initialisierungsfunktion
*/

typedef void (*setup_t) (void);

/*	[|$1|]	Identifikationskennung
*/

typedef char *(*ident_t) (const void *data);

/*	[|$1|]	Datenobjekt einrichten
*/

typedef void *(*create_t) (void);

/*	[|$1|]	Datenobjekt freigeben
*/

typedef void (*clean_t) (void *ptr);

/*	[|$1|]	Datenobjekt kopieren
*/

typedef void *(*copy_t) (const void *src);


/*	[|$1|]
		Datenobjekt verwalten, integriert
		|create_t|, |copy_t| und |clean_t|.
*/

typedef void *(*admin_t) (void *tg, const void *src);

/*	Kettenstruktur |$1|
*/

typedef struct chain_s {
	struct chain_s *next;	/* Verweis auf Nachfolger */
} chain_t;

/*	Label |$1|
*/

typedef struct {
	char *name;
	char *desc;
} Label_t;

#ifndef	sizealign

/*
Der Makro |$1| dient zur Ausrichtung der Speichergröße <x> auf
ganzzahlige Vielfache von <y>.
*/

#define	sizealign(x, y)	((y) * (((x) + (y) - 1) / (y)))
#endif

#ifndef	tabsize

/*
Der Makro |$1| liefert die erste Dimension des Datenfeldes <x>.  Er wird für
globale oder statische Felder verwendet, deren erste Dimension nicht
vorgegebnen ist, sondern aus der Zahl der Initialisierungswerte bestimmt  wird.
*/

#define	tabsize(x)	(sizeof(x) / sizeof(x[0]))
#endif

#endif	/* EFEU/types.h */
