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

#define	TRACE_MODE	1	/* enable trace control */

#define	DEF_SIZE	2040
#define	MIN_BLK		16

static int log_lock = 0;
static LogControl log_debug = LOG_CONTROL("alloctab", LOGLEVEL_DEBUG);

static ALLOCTAB(root, "root", 0, 0);	/* Dummy-Tabelle für verkettung */
static AllocTab *chain = &root;

/*
$Description
:*:This functions administrates memmory segments of fixed size
with an assignment table. The internal structure is described
in \mref{memalloc(7)}.
:de:Die Funktionen verwalten Speichersegmente fixer Größe über eine
Zuweisungstabelle. Der innere Aufbau und die Einrichtung einer
Zuweisungstabelle sind in \mref{memalloc(7)} beschrieben.
*/

#define	TRACE_NEW "Pointer %p allocated.\n"
#define	TRACE_DEL "Pointer %p released.\n"

static void alloctab_id (IO *out, AllocTab *tab)
{
	io_puts("alloctab(", out);

	if	(tab->name)
	{
		io_puts(tab->name, out);
	}
	else	io_printf(out, "%p", tab);

	io_puts("): ", out);
}

#if	TRACE_MODE

static LogControl log_trace = LOG_CONTROL("alloctab", LOGLEVEL_TRACE);

static void alloctab_trace (AllocTab *tab, const char *fmt, void *ptr)
{
	IO *out;

	if	(log_lock)	return;

	log_lock = 1;

	if	((out = LogOpen(&log_trace)))
	{
		alloctab_id(out, tab);
		io_printf(out, fmt, ptr);
		io_close(out);
	}

	log_lock = 0;
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
	IO *out;
	
	if	(log_lock)	return;

	log_lock = 1;

	if	((out = LogOpen(ErrLog)))
	{
		va_list args;

		alloctab_id(out, tab);
		va_start(args, fmt);
		io_vprintf(out, fmt, args);
		va_end(args);
		io_close(out);
	}

	log_lock = 0;
}


static void load (AllocTab *tab)
{
	size_t n, size;
	AllocTabList *p;

	if	(!tab->chain)
	{
		tab->chain = chain;
		chain = tab;
	}

	if	(!tab->blksize)
	{
		tab->blksize = DEF_SIZE / tab->elsize;

		if	(tab->blksize < MIN_BLK)
			tab->blksize = MIN_BLK;
	}

	tab->nfree = tab->blksize;
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
list may be destroyed.
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

	n = tab->elsize * tab->blksize;

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

static void alloctab_abort (AllocTab *tab, const char *fmt, ...)
{
	IO *out = LogOpen(&log_debug);

	if	(out)
	{
		va_list args;

		va_start(args, fmt);
		alloctab_id(out, tab);
		io_vprintf(out, fmt, args);
		va_end(args);
		io_close(out);
		abort();
	}
}

/*
:*:The function |$1| checks the consistence of a allocation
table. The acitivities are only performed, if the debug_level
for alloctab is at least debug (see \mref{Debug(3)}).
:de:Die Funktion |$1| überprüft die Konsistenz der Zuweisungstabelle und
ihrer Speichersegmente falls der Debug-Level für alloctab
auf debug gesetzt ist.
Vergleiche dazu auch \mref{Debug(3)}.
*/

void check_data (AllocTab *tab)
{
	AllocTabList *p;
	size_t n;

	if	(!tab || log_lock)	return;

	log_lock = 1;
	LogUpdate(&log_debug);
	log_lock = 0;

	if	(!log_debug.entry)	return;

	log_lock = 1;

	for (n = 0, p = tab->blklist; p; n += tab->blksize, p = p->next)
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

	log_lock = 0;
}

/*
:*:The function |$1| walks along the table chain and calls <visit> vor every
allocation tabel. It's use is primary for debugging and generating statistics
about memory usage.
:de:Die Funktion |$1| durchwandert die Tabellenkette und ruft <visit> für
jede Tabelle auf. Ihre Verwendung dient primär der Fehlersuche und der
Generierung von Statistiken über die Speicherverwendung.
*/

void AllocTab_walk (void (*visit) (AllocTab *tab, void *par), void *par)
{
	AllocTab *tab;

	if	(!visit)	return;

	for (tab = chain; tab; tab = tab->chain)
		visit(tab, par);
}

/*
$Notes
:*:This functions relieves the dynamic memmory allocation and
reduces fragmentation by small objects.
:de:Die Funktionen entlasten die dynamische Speicherplatzverwaltung
und verhindern eine Zerstückelung von Speichersegmenten bei kleinen
Datenobjekten.

$SeeAlso
\mref{memalloc(3)}, \mref{memalloc(7)}.\br
\mref{malloc(3C)} @PRM.
*/
