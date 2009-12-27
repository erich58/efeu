/*
:*:memmory allocation
:de:Speicherplatzanforderung

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
Bei Problemen mit dynamisch angeforderten Speichersegmenten
kann der Makro MEMCHECK auf 1 gesetzt werden. Jedes Segment
wird zusätzlich mit einer Anfangs- und Endekennung versehen.
Bei der Freigabe werden diese Kennungen überprüft.

Falls die Speicherplatzanforderung fehlschlägt, kann nicht
auf die Standardfehlerroutinen zurückgegriffen werden, da
diese eventuell selbst ein Speichersegment anfordern müssen.
*/

#define	MEMCHECK	0
#define	MEMTRACE	1

#define	ERR	"sorry: malloc(%lu) failed\n"
#define	ERR2	"sorry: realloc(%p, %lu) failed\n"


#if	MEMCHECK

#define	CHECK_SIZE	8
#define	CHECK_MASK	"12345678"

#define	MSG1	"lcheck(%p): start marker destroyed.\n"
#define	MSG2	"lcheck(%p): end marker destroyed.\n"

typedef struct {
#if	REFBYTEORDER
	size_t size;
	char check[CHECK_SIZE];
#else
	char check[CHECK_SIZE];
	size_t size;
#endif
} MEMHEAD;

static void *checkmalloc (size_t size)
{
	MEMHEAD *head;
	char *p;

	head = malloc(size + sizeof(MEMHEAD) + CHECK_SIZE);

	if	(head)
	{
		p = (char *) (head + 1);
		head->size = size;
		memcpy(head->check, CHECK_MASK, CHECK_SIZE);
		memcpy(p + size, CHECK_MASK, CHECK_SIZE);
		return p;
	}
	else	return NULL;
}

static void *checkrealloc (void *ptr, size_t size)
{
	size_t osize;
	void *p;
	
	osize = ((MEMHEAD *) ptr)[-1].size;

	if	(osize > size)	osize = size;

	p = checkmalloc(size);
	memcpy(p, ptr, osize);
	lfree(ptr);
	return p;
}

#endif

/*
Die Variable |$1| dient zur Aktivierung der Ablaufkontrolle
für die Speicherverwaltung.
*/

int memtrace = 0;

#if	MEMTRACE

#define	TRACE_BSIZE	1000

typedef struct {
	void *ptr;
	unsigned idx;
	size_t size;
	int change;
} TRACE;

static TRACE *trace_tab = NULL;
static size_t trace_size = 0;
static size_t trace_dim = 0;
static unsigned trace_idx = 0;

static void trace_add (void *ptr, size_t size)
{
	int i, i1, i2;

	if	(!memtrace)	return;

	i1 = 0;
	i2 = trace_dim;
	trace_idx++;

	while (i1 != i2)
	{
		i = (i1 + i2) / 2;

		if	(trace_tab[i].ptr < ptr)	i1 = i + 1;
		else if	(trace_tab[i].ptr > ptr)	i2 = i;
		else
		{
			trace_tab[i].idx = trace_idx;
			trace_tab[i].size = size;
			trace_tab[i].change++;
			return;
		}
	}

	if	(trace_dim >= trace_size)
	{
		TRACE *save = trace_tab;

		trace_size += TRACE_BSIZE;
		trace_tab = malloc(trace_size * sizeof(TRACE));

		if	(!trace_tab)
		{
			fprintf(stderr, "malloc(%lu) failed.\n",
				(unsigned long) trace_size * sizeof(TRACE));
			exit(EXIT_FAILURE);
		}

		for (i = 0; i < i1; i++)
			trace_tab[i] = save[i];

		for (i = trace_dim; i > i1; i--)
			trace_tab[i] = save[i - 1];

		if	(save)
			free(save);
	}
	else
	{
		for (i = trace_dim; i > i1; i--)
			trace_tab[i] = trace_tab[i - 1];
	}

	trace_tab[i1].ptr = ptr;
	trace_tab[i1].idx = trace_idx;
	trace_tab[i1].size = size;
	trace_tab[i1].change = 1;
	trace_dim++;
}
#endif

/*
Die Funktion |$1| listet alle noch in Verwendung stehenden
Speicherfelder auf.
*/

void meminfo (const char *pfx)
{
#if	MEMTRACE
	TRACE *p;
	size_t n;

	if	(!pfx)	pfx = "meminfo()";

	for (p = trace_tab, n = trace_dim; n-- > 0; p++)
	{
		if	(p->size)
		{
			fprintf(stderr, "%s:\t%p\t%u\t%lu\n", pfx,
				p->ptr, p->idx, (unsigned long) p->size);
		}
	}
#else
	;
#endif
}

/*
Die Funktion |$1| listet alle Änderungen in der Speicherverwaltung
seit dem letzten Aufruf auf.
*/

void memchange (const char *pfx)
{
#if	MEMTRACE
	TRACE *p;
	size_t n;

	if	(!pfx)	pfx = "memchange()";

	for (p = trace_tab, n = trace_dim; n-- > 0; p++)
	{
		if	(p->change)
		{
			p->change = 0;
			fprintf(stderr, "%s:\t%p\t%u\t%lu\n", pfx,
				p->ptr, p->idx, (unsigned long) p->size);
		}
	}
#else
	;
#endif
}

/*
:de:
Die Funktion |$1| reserviert einen Speicherplatz der
Größe <size>. Durch |lfree| kann der Speicherplatz
wieder freigegeben werden.

$Warnings
:*:
If the end of mewomory
:de:
Falls über das Ende eines reservierten Speicherplatzes
hinausgeschrieben wird, kommt
es zu einem unkontrollierten Fehlverhalten.
Meistens führt ein späterer Aufruf von |$1| zu einem
Absturz des Programms.

$Diagnostics
:de:
Wird ein Speicherfeld der Größe 0 angefordert, liefert
die Funktion |$1| einen Nullpointer.
Kann der gewünschte Speicherplatz nicht reserviert werden,
wird die Verarbeitung des rufenden Kommandos mit einer Fehlermeldung
abgebrochen. Der Rückgabewert von |$1| muß daher nicht überprüft
werden.
*/

void *lmalloc (size_t size)
{
	void *p;

	if	(size == 0)	return NULL;

#if	MEMCHECK
	if	((p = checkmalloc(size)) == NULL)
#else
	if	((p = malloc(size)) == NULL)
#endif
	{
		fprintf(stderr, ERR, (unsigned long) size);
		exit(EXIT_FAILURE);
	}

#if	MEMTRACE
	trace_add(p, size);
#endif
	return p;
}

/*
Die Funktion |$1| gibt einen von |lmalloc| angeforderten
Speicherbereich wieder frei. Ein Nullpointer ist als Argument zulässig.
*/

void lfree (void *p)
{
#if	MEMCHECK
	lcheck(p);
#else
	if	(p != NULL)
		free(p);
#endif
#if	MEMTRACE
	if	(p)
		trace_add(p, 0);
#endif
}

/*
Die Funktion |$1| ändert die Größe eines zuvor mit |lmalloc| angeforderten
Speicherfeldes auf <size> und liefert die Adresse des möglicherweise
verschobenen Speicherfeldes. Falls als <ptr> ein Nullpointer übergeben wurde,
ist die Funktion äquivalent zu |lmalloc|, falls <size> 0 ist, ist die
Funktion äquivalent zu |lfree| und liefert einen Nullpointer.
*/

void *lrealloc (void *ptr, size_t size)
{
	char *p;

	if	(ptr == NULL)	return lmalloc(size);
	if	(size == 0)	return lfree(ptr), NULL;

#if	MEMCHECK
	if	((p = checkrealloc(ptr, size)) == NULL)
#else
	if	((p = realloc(ptr, size)) == NULL)
#endif
	{
		fprintf(stderr, ERR2, ptr, (unsigned long) size);
		exit(EXIT_FAILURE);
	}

#if	MEMTRACE
	trace_add(ptr, 0);
	trace_add(p, size);
#endif
	return p;
}


/*
Die Funktion |$1| testet ein Speichersegment, ob sein Anfang oder sein
Ende überschrieben wurde. Vorraussetzung ist dabei, daß vorm Kompilieren
der Makro |MEMCHECK| auf 1 gesetzt wurde.
*/

void lcheck (void *p)
{
#if	MEMCHECK
	MEMHEAD *head;

	if	(p == NULL)	return;

	head = ((MEMHEAD *) p) - 1;

	if	(memcmp(head->check, CHECK_MASK, CHECK_SIZE) != 0)
		fprintf(stderr, MSG1, p);

	if	(memcmp(((char *) p) + head->size, CHECK_MASK, CHECK_SIZE) != 0)
		fprintf(stderr, MSG2, p);
#else
	;
#endif
}

/*
$Notes
Für kleine Speichersegmente sollten
die Funktionen |memalloc| und |memfree| eingesetzt
werden.

$Warnings
Bei Übergabe eines Pointers an die Funktion
|lfree|, der nicht durch einen vorangegangenen Aufruf von
|lmalloc| stammt, ist das Verhalten unbestimmt und kann
zu einem Programmabsturz führen.
Die Funktionen |memalloc| und |memfree| sind diesbezüglich robust.
$SeeAlso
\mref{alloctab(3)}, \mref{memalloc(3)}.\br
\mref{malloc(3S)} @PRM.
*/
