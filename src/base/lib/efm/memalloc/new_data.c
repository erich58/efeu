/*
Verwaltung von Speichersegmenten fixer Größe

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

#include <EFEU/memalloc.h>

/*
$Description
Die Funktionen verwalten Speichersegmente fixer Größe über eine
Zuweisungstabelle. Der innere Aufbau iund die Einrichtung einer
Zuweisungstabelle sind in \mref{memalloc(7)} beschrieben.

$local
Die freie Liste ist nach  der Größe nach absteigend sortiert.
Damit kann der Test auf Mehrfachfreigabe schneller durchgeführt
werden.
*/

#define	DEBUG_MODE	0	/* Zusätzliche Protokolle für new und del */

#define	DEF_BLK		16

#define	BLKSIZE(tab)	((tab)->blksize ? (tab)->blksize : DEF_BLK)

#if	DEBUG_MODE
#define	NOTE_NEW "Pointer %p angefordert.\n"
#define	NOTE_DEL "Pointer %p freigegeben.\n"
#endif

#define	ERR_FREE "Pointer %p bereits freigegeben.\n"
#define	ERR_ADDR "Pointer %p ist keine Segmentadresse (Offset %d).\n"

#define	ERR_SIZE	"Zuweisungsgrößen %lu != %lu + %lu.\n"
#define	ERR_LFREE	"Zuwenig Elemente in der freien Liste.\n"
#define	ERR_MFREE	"Zuviele Elemente in der freien Liste.\n"
#define	ERR_ILLEGAL	"Freier Eintrag %p ist nicht in der Blockliste\n"

#define	IGNORE	0
#define	EXIT	1
#define	ABORT	2

static void alloctab_error (alloctab_t *tab, int mode, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "alloctab %p: ", tab);
	vfprintf(stderr, fmt, args);
	va_end(args);

	switch (mode)
	{
	case EXIT:	exit(EXIT_FAILURE); break;
	case ABORT:	abort(); break;
	default:	break;
	}
}


static void load (alloctab_t *tab)
{
	size_t n, size;
	chain_t *p;

	tab->nfree = BLKSIZE(tab);
	size = tab->elsize * tab->nfree;
	p = (chain_t *) lmalloc(sizeof(chain_t) + size);
	p->next = tab->blklist;
	tab->blklist = p;
	p++;
	memset(p, 0, size);
	tab->freelist = NULL;

	for (n = 0; n < tab->nfree; n++)
	{
		p->next = tab->freelist;
		tab->freelist = p;
		p = (chain_t *) ((char *) p + tab->elsize);
	}
}


/*
Die Funktion |$1| stellt ein neues Datensegment zur Verfügung.

$Diagnostics
Kann |$1| kein neues Segment mehr zur Verfügung stellen,
wird die Verarbeitung des rufenden Kommandos mit einer Fehlermeldung
abgebrochen.
*/

void *new_data (alloctab_t *tab)
{
	if	(tab != NULL)
	{
		chain_t *x;

		if	(alloctab_debug_flag)
			check_data(tab);

		if	(tab->nfree == 0)
			load(tab);

		x = tab->freelist;
		tab->freelist = x->next;
		memset(x, 0, tab->elsize);
		tab->nfree--;
		tab->nused++;

#if	DEBUG_MODE
		if	(alloctab_debug_flag)
			alloctab_error(tab, IGNORE, NOTE_NEW, (void *) x);
#endif

		return (void *) x;
	}

	return NULL;
}

/*
Die Funktion |$1| gibt das Datensegment <ptr> wieder frei.
Dabei wird überprüft, ob das Datensegment aus der Zuweisungstabelle
stammt, eine gültige Adresse besitzt und nicht bereits in der
Liste der freien Elemente aufscheint.
Die Funktion liefert 1 bei Erfolg und 0 falls der Pointer nicht
aus der Zuweisungstabelle stammt.

Anstelle von <ptr> kann
auch ein Nullpointer übergeben werden, in diesem Fall werden
von der Funktion |$1| keine Aktionen durchgeführt.

$Diagnostics
Falls |$1| mit einem Datenpointer <ptr> verschieden von Null
aufgerufen wurde, der nicht von einem Aufruf von |new_data| mit derselben
Zuweisungstabelle stammt, liefert die Funktion den Wert 0, ansonsten 1.

$Warnings
Nach der Freigabe eines Datensegmentes darf dieses nicht mehr benützt werden,
da ansonsten die freie Liste zerstört wird.
Der Inhalt eines Datensegmentes wird bei der Freigabe verändert.
*/

int del_data (alloctab_t *tab, void *entry)
{
	chain_t *x, **ptr;

	if	(entry == NULL)
		return 1;

	if	(alloctab_debug_flag)
		check_data(tab);

	if	(!tst_data(tab, entry))
		return 0;

#if	DEBUG_MODE
	if	(alloctab_debug_flag)
		alloctab_error(tab, IGNORE, NOTE_DEL, entry);
#endif

	x = entry;
	ptr = &tab->freelist;

	while (x < *ptr)
		ptr = &(*ptr)->next;

	if	(x == *ptr)
	{
		alloctab_error(tab, IGNORE, ERR_FREE, entry);
		return 0;
	}

	x->next = *ptr;
	*ptr = x;
	tab->nfree++;
	tab->nused--;
	return 1;
}

/*
Die Funktion |$1| überprüft einen Pointer, ob er aus der
Zuweisungstabelle stammt und eine gültige Adressse hat.
*/

int tst_data (alloctab_t *tab, void *entry)
{
	int n;
	chain_t *p;
	void *a, *b;

	if	(tab == NULL)	return 0;

	n = tab->elsize * BLKSIZE(tab);

	for (p = tab->blklist; p != NULL; p = p->next)
	{
		a = p + 1;
		b = (char *) a + n;

		if	(entry < a || entry >= b)	continue;

		n = ((size_t) entry - (size_t) a) % tab->elsize;

		if	(n != 0)
			alloctab_error(tab, EXIT, ERR_ADDR, entry, n);

		return 1;
	}

	return 0;
}

/*
Die Funktion |$1| überprüft die Konsistenz der Zuweisungstabelle und
ihrer Speichersegmente. Sie wird nur für die Fehlersuche verwendet.
Vergleiche dazu auch \mref{lmalloc}.
*/

void check_data (alloctab_t *tab)
{
	chain_t *p;
	size_t n;

	if	(tab == NULL)	return;

	for (n = 0, p = tab->blklist; p != NULL; n += BLKSIZE(tab), p = p->next)
		lcheck(p);

	if	(n != tab->nused + tab->nfree)
		alloctab_error(tab, EXIT, ERR_SIZE, n, tab->nused, tab->nfree);

	for (n = 0, p = tab->freelist; p != NULL; p = p->next, n++)
	{
		if	(n >= tab->nfree)
			alloctab_error(tab, ABORT, ERR_MFREE, tab->nfree);

		if	(!tst_data(tab, p))
			alloctab_error(tab, ABORT, ERR_ILLEGAL, p);
	}

	if	(n != tab->nfree)
		alloctab_error(tab, ABORT, ERR_LFREE, tab->nfree);
}

/*
Die Variable |$1| dient zur Kontrolle der Speicherverwaltung.
Wird sie auf einen Wert verschieden von 0 gesetzt,
werden die einzelnen Funktionsaufrufe protokolliert.
*/

int alloctab_debug_flag = 0;

/*
$Notes
Die Funktionen entlasten die dynamische Speicherplatzverwaltung
und verhindern eine Zerstückelung von Speichersegmenten bei kleinen
Datenobjekten.

$SeeAlso
\mref{memalloc(3)}, \mref{memalloc(7)}.\br
\mref{malloc(3C)} im @PRM.
*/
