/*
:*:administration of memmory segments fixed size.
:de:Verwaltung von Speichersegmenten fixer Größe

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
#include <EFEU/ftools.h>
#include <EFEU/Debug.h>

#define	TRACE_MODE	0	/* enable trace control */

#define	DEF_BLK		16

#define	BLKSIZE(tab)	((tab)->blksize ? (tab)->blksize : DEF_BLK)

/*
$Description
:*:This functions administrates memmory segments of fixed size
with an assignment table. The internal structure is described
in \mref{memalloc(7)}.
:de:Die Funktionen verwalten Speichersegmente fixer Größe über eine
Zuweisungstabelle. Der innere Aufbau iund die Einrichtung einer
Zuweisungstabelle sind in \mref{memalloc(7)} beschrieben.
*/

#define	TRACE_NEW "Pointer %p requisited.\n"
#define	TRACE_DEL "Pointer %p released.\n"

#if	TRACE_MODE

static void alloctab_trace (AllocTab *tab, const char *fmt, void *ptr)
{
	static FILE *trace_file = NULL;
	static int trace_sync = 0;

	if	(trace_sync < DebugChangeCount)
	{
		trace_sync = DebugChangeCount;
		fileclose(trace_file);
		trace_file = filerefer(LogFile("alloctab", DBG_TRACE));
	}

	if	(trace_file)
	{
		fprintf(trace_file, "alloctab %p: ", tab);
		fprintf(trace_file, fmt, ptr);
	}
}

#else
#define	alloctab_trace(tab, fmt, ptr)
#endif

#define	ERR_FREE "Pointer %p already released.\n"
#define	ERR_ADDR "Pointer %p is not a legal address (offset %d).\n"

#define	ERR_SIZE	"assignment parameters: %lu != %lu + %lu.\n"
#define	ERR_LFREE	"missing entries in free list.\n"
#define	ERR_MFREE	"too many entries in used list.\n"
#define	ERR_ILLEGAL	"free entry %p is not part of block list\n"


static void alloctab_error (AllocTab *tab, const char *fmt, ...)
{
	FILE *file = LogFile("alloctab", DBG_ERR);

	if	(file)
	{
		va_list args;

		va_start(args, fmt);
		fprintf(file, "alloctab %p: ", tab);
		vfprintf(file, fmt, args);
		va_end(args);
	}
}


static void load (AllocTab *tab)
{
	size_t n, size;
	AllocTabList *p;

	tab->nfree = BLKSIZE(tab);
	size = tab->elsize * tab->nfree;
	p = (AllocTabList *) lmalloc(sizeof(AllocTabList) + size);
	p->next = tab->blklist;
	tab->blklist = p;
	p++;
	memset(p, 0, size);
	tab->freelist = NULL;

	for (n = 0; n < tab->nfree; n++)
	{
		p->next = tab->freelist;
		tab->freelist = p;
		p = (AllocTabList *) ((char *) p + tab->elsize);
	}
}


/*
:*:The Function |$1| provides a new data segment.
:de:Die Funktion |$1| stellt ein neues Datensegment zur Verfügung.

$Diagnostics
:*:If |$1| could not allocate a new segment, the programm
is terminated.
:de:Kann |$1| kein neues Segment mehr zur Verfügung stellen,
wird die Verarbeitung des rufenden Kommandos mit einer Fehlermeldung
abgebrochen.
*/

void *new_data (AllocTab *tab)
{
	if	(tab != NULL)
	{
		AllocTabList *x;

		check_data(tab);

		if	(tab->nfree == 0)
			load(tab);

		x = tab->freelist;
		tab->freelist = x->next;
		memset(x, 0, tab->elsize);
		tab->nfree--;
		tab->nused++;
		alloctab_trace(tab, TRACE_NEW, (void *) x);
		return (void *) x;
	}

	return NULL;
}

/*
:*:The Function |$1| releases the datasegment <ptr>, if it
has a legal address and was not released bevor. It is save
to call |$1| with a pointer not recieved from the table.
:de:Die Funktion |$1| gibt das Datensegment <ptr> wieder frei.
Dabei wird überprüft, ob das Datensegment aus der Zuweisungstabelle
stammt, eine gültige Adresse besitzt und nicht bereits in der
Liste der freien Elemente aufscheint.
Die Funktion liefert 1 bei Erfolg und 0 falls der Pointer nicht
aus der Zuweisungstabelle stammt.

:de:Anstelle von <ptr> kann
auch ein Nullpointer übergeben werden, in diesem Fall werden
von der Funktion |$1| keine Aktionen durchgeführt.

$Diagnostics
:*:The function |$1| returns 1 if the pointer
was not provided by the table and is different from NULL.
In all other cases, the return value is 0.
:de:Falls |$1| mit einem Datenpointer <ptr> verschieden von Null
aufgerufen wurde, der nicht von einem Aufruf von |new_data| mit derselben
Zuweisungstabelle stammt, liefert die Funktion den Wert 0, ansonsten 1.

$Warnings
:*:If a data segment is used after freeing, the internal free
list may be destroied.
:de:Nach der Freigabe eines Datensegmentes darf dieses nicht mehr
benützt werden, da ansonsten die freie Liste zerstört wird.
Der Inhalt eines Datensegmentes wird bei der Freigabe verändert.
*/

int del_data (AllocTab *tab, void *entry)
{
	AllocTabList *x, **ptr;

	if	(entry == NULL)
		return 1;

	check_data(tab);

	if	(!tst_data(tab, entry))
		return 0;

	alloctab_trace(tab, TRACE_DEL, entry);

	x = entry;
	ptr = &tab->freelist;

	while (x < *ptr)
		ptr = &(*ptr)->next;

	if	(x == *ptr)
	{
		alloctab_error(tab, ERR_FREE, entry);
		return 0;
	}

	x->next = *ptr;
	*ptr = x;
	tab->nfree++;
	tab->nused--;
	return 1;
}

/*
:*:The Function |$1| checks if <entry> descend from the
table <tab>
:de:Die Funktion |$1| überprüft einen Pointer, ob er aus der
Zuweisungstabelle stammt und eine gültige Adressse hat.
*/

int tst_data (AllocTab *tab, void *entry)
{
	int n;
	AllocTabList *p;
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
		{
			alloctab_error(tab, ERR_ADDR, entry, n);
			exit(EXIT_FAILURE);
		}

		return 1;
	}

	return 0;
}

static int debug_sync = 0;
static FILE *debug_file = NULL;

static void alloctab_abort (AllocTab *tab, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(debug_file, "alloctab %p: ", tab);
	vfprintf(debug_file, fmt, args);
	va_end(args);
	abort();
}

/*
:*:The function |$1| checks the consistence of a allocation
table. The acitivities are only performed, if the debug_level
for alloctab is at least debug (see \mref{Debug(3)}).
:de:Die Funktion |$1| überprüft die Konsistenz der Zuweisungstabelle und
ihrer Speichersegmente falls dere Debug-Level für alloctab
auf debug gesetzt ist.
Vergleiche dazu auch \mref{Debug(3)}.
*/

void check_data (AllocTab *tab)
{
	AllocTabList *p;
	size_t n;

	if	(tab == NULL)	return;

	if	(debug_sync < DebugChangeCount)
	{
		debug_sync = DebugChangeCount;
		fileclose(debug_file);
		debug_file = filerefer(LogFile("alloctab", DBG_DEBUG));
	}

	if	(!debug_file)	return;

	for (n = 0, p = tab->blklist; p != NULL; n += BLKSIZE(tab), p = p->next)
		lcheck(p);

	if	(n != tab->nused + tab->nfree)
		alloctab_abort(tab, ERR_SIZE, n, tab->nused, tab->nfree);

	for (n = 0, p = tab->freelist; p != NULL; p = p->next, n++)
	{
		if	(n >= tab->nfree)
			alloctab_abort(tab, ERR_MFREE, tab->nfree);

		if	(!tst_data(tab, p))
			alloctab_abort(tab, ERR_ILLEGAL, p);
	}

	if	(n != tab->nfree)
		alloctab_abort(tab, ERR_LFREE, tab->nfree);
}

/*
$Notes
:*:This functions relievs the dynamic memmory allocation and
reduces fragmentation by small objects.
:de:Die Funktionen entlasten die dynamische Speicherplatzverwaltung
und verhindern eine Zerstückelung von Speichersegmenten bei kleinen
Datenobjekten.

$SeeAlso
\mref{memalloc(3)}, \mref{memalloc(7)}.\br
\mref{malloc(3C)} @PRM.
*/
