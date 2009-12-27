/*
Robuste Speicherplatzzuweisung

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
#include <EFEU/vecbuf.h>

#define	MEM_SMALL	8	/* Kleine Datenblöcke */
#define	MEM_MEDIUM	32	/* Mittlere Datenblöcke */

#define	WARN_1	"memalloc(): Pointer reused without freeing\n"
#define	STAT_1	"%s: Blocksize %3ld: %5ld used, %5ld free, %5ld byte (%ldx%ldx%ld)\n"
#define	STAT_L	"%s: Large blocks:  %5ld used, %5ld requests\n"

static int mem_cmp (const void *a, const void *b);

/*	Zuweisungstabellen: Diese müssen mit aufsteigender Elementlänge
	angeordnet werden.
*/

static AllocTab mem_tab[] = {
	ALLOCDATA(512, 1 * sizeof(void *)),
	ALLOCDATA(256, 2 * sizeof(void *)),
	ALLOCDATA(128, 4 * sizeof(void *)),
	ALLOCDATA(64, 8 * sizeof(void *)),
	ALLOCDATA(32, 16 * sizeof(void *)),
};

static VECBUF (mem_large, 32, sizeof(void *));
static size_t request = 0;


/*	Vergleichsfunktion für Speichersegmente
*/

static int mem_cmp (const void *pa, const void *pb)
{
	void * const *a = pa;
	void * const *b = pb;

	if	(*a < *b)	return -1;
	else if	(*a > *b)	return 1;
	else			return 0;
}

/*
Die Funktion |$1| reserviert einen Speicherplatz der
Größe <size>. Sie verwaltet unterschiedliche
Zuweisungstabellen in Abhängigkeit der Größe des Speicherfeldes.

$Diagnostic
Kann der gewünschte Speicherplatz nicht reserviert werden,
wird die Verarbeitung mit einer Fehlermeldung abgebrochen.

$Note
Ein Anwendungsprogramm, daß viele gleich lange Speichersegmente
verwendet, sollte diese nach Möglichkeit direkt mit \mref{alloctab(3)}
verwalten, da damit der Zuweisungsalgorithmus von
|$1| entlastet wird.
*/

void *memalloc (size_t size)
{
	void *ptr;
	int i;

	if	(size == 0)	return NULL;

	for (i = 0; i < tabsize(mem_tab); i++)
		if (size <= mem_tab[i].elsize)
			return new_data(mem_tab + i);

	ptr = lmalloc(size);
	memset(ptr, 0, size);
	memnotice(ptr);
	return ptr;
}


/*
Die Funktion |$1| gibt einen mit |memalloc|
generierten Speicherplatz frei. Die Funktion überprüft den
Speicherplatz <ptr>, ob er aus den Zuweisungstabellen
stammt. Die Übergabe eines Pointers, der <nicht>
von einem Aufruf von |memalloc| stammt oder mit
|memnotice| registriert wurde, ist zulässig.
*/

void memfree (void *p)
{
	int i;

	if	(p == NULL)	return;

	if	(vb_search(&mem_large, &p, mem_cmp, VB_DELETE))
	{
		lfree(p);
		return;
	}

	for (i = 0; i < tabsize(mem_tab); i++)
	{
		if	(tst_data(mem_tab + i, p))
		{
			del_data(mem_tab + i, p);
			return;
		}
	}
}

/*
Die Funktion |$1| registriert ein mit |lmalloc|
oder |malloc| generiertes Speicherfeld und ermöglicht damit
die Freigabe des Speicherfeldes mit |memfree|.
$diagnostic
Falls |$1| zweimal mit dem selben Datenpointer
aufgerufen wird, ohne daß er zuvor freigegeben wurde,
wird eine Warnung ausgegeben.
*/

void memnotice (void *ptr)
{
	if	(ptr)
	{
		request++;

		if	((vb_search(&mem_large, &ptr, mem_cmp, VB_REPLACE)))
			fprintf(stderr, WARN_1);
	}
}

/*
Die Funktion |$1| schreibt eine Liste der
von |memalloc| reservierten Speichersegmete
zur Standardfehlerausgabe.
*/

void memstat (const char *prompt)
{
	int i;

	if	(!prompt)	prompt = "memstat()";

	for (i = 0; i < tabsize(mem_tab); i++)
	{
		AllocTabList *x;
		size_t n;

		x = mem_tab[i].blklist;

		for (n = 0; x != NULL; n++)
			x = x->next;

		fprintf(stderr, STAT_1, prompt,
			(unsigned long) mem_tab[i].elsize,
			(unsigned long) mem_tab[i].nused,
			(unsigned long) mem_tab[i].nfree,
			(unsigned long) n * mem_tab[i].blksize * mem_tab[i].elsize,
			(unsigned long) n,
			(unsigned long) mem_tab[i].blksize,
			(unsigned long) mem_tab[i].elsize);
	}

	fprintf(stderr, STAT_L, prompt,
		(unsigned long) mem_large.used, (unsigned long) request);
}

/*
Die Funktion |$1| überprüft die einzelnen Zuweisungstabellen
auf Konsistenz. Sie wird nur für die Fehlersuche verwendet.
*/

void memcheck (void)
{
	int i;
	void **ptr;

	for (i = 0; i < tabsize(mem_tab); i++)
		check_data(mem_tab + i);

	ptr = mem_large.data;

	for (i = 0; i < mem_large.used; i++)
		lcheck(ptr[i]);
}

/*
$SeeAlso
\mref{alloctab(3)}, \mref{lmalloc(3)}.\br
\mref{malloc(3S)}, \mref{stdio(3S)} @PRM.
*/
