/*	Speicherplatzverwaltung
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_MEMALLOC_H
#define	EFEU_MEMALLOC_H	1

#include <EFEU/config.h>
#include <EFEU/types.h>


/*	Elementare Speicherzuweisung
*/

void *lmalloc (size_t size);
void lfree (void *ptr);
void lcheck (void *ptr);


/*
Die Datenstruktur |$1| definiert eine Speichersegmenttabelle zur
Verwaltung von Speichersegmenten der Größe |elsize|.

Sie enthält eine Liste der freien Elemente
(|freelist|) und eine Liste mit Datenblöcken (|blklist|).
Die Zahl der freien Segmente wird in |nfree| und die Zahl der
benutzten Segmente wird in |nused| gespeichert.
Mit den Funktionen |new_data| und |del_data| können Datensegmente
entnommen und wieder hineiungestellt werden. Vergleiche dazu
\mref{alloctab(3)}.

Zum Auffüllen der freien Liste wird ein Block mit |blksize| Elementen
angefordert. Jedem Block ist eine Kettenstruktur vorangestellt (Für
die Liste der Datenblöcke).  Der Speicherbedarf für einen Block
beträgt |sizeof(chain_t)| + |blksize| * |elsize|.
Datenblöcke, die zum Füllen der freien Liste angefordert wurden,
werden nicht mehr freigegeben.

Die Datensegmentgröße muß auf die Größe einer Kettenstruktur
ausgerichtet sein.  Freie Elemente werden zu einer Liste verkettet und
können dort entnommen werden.
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

extern int alloctab_debug_flag;

#define	ALLOCSIZE(size)		sizealign(size, sizeof(chain_t))
#define	ALLOCDATA(blk,size)	{ blk, ALLOCSIZE(size), 0, 0, NULL, NULL, }

/*
Der Makro |$1| initialisiert eine Zuweisungstabelle
zur Verwaltung von Speicherelementen fixer Länge unter dem Namen <name>.
Die Länge eines Segmentes wird durch <elsize> festgelegt.
Durch <blksize> wird die Zahl der Elemente eines Blockes festgelegt.
Falls <blksize> den Wert 0 hat, werden Standardvorgaben verwendet.
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
