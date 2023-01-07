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
#include <EFEU/Debug.h>

static void *(*realloc_function)(void *data, size_t size) = NULL;

void lmcfg(void *(*realloc)(void *ptr, size_t size))
{
	realloc_function = realloc;
}

static LogControl log_trace = LOG_CONTROL("lmalloc", LOGLEVEL_TRACE);

static void lmalloc_trace (void *ptr, size_t size)
{
	static int lock = 0;

	if	(lock)	return;

	lock = 1;
	log_printf(&log_trace, "lmalloc: %p %zd byte\n", ptr, size);
	lock = 0;
}

/*
:de:
Die Funktion |$1| reserviert einen Speicherplatz der
Größe <size>. Durch |lfree| kann der Speicherplatz
wieder freigegeben werden.

$Warnings
:*:
If the end of the memory segment is overwritten and may result in
a segmentation fault on a later call to |$1|.
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
	return lrealloc(NULL, size);
}

/*
Die Funktion |$1| gibt einen von |lmalloc| oder |lrealloc| angeforderten
Speicherbereich wieder frei. Ein Nullpointer ist als Argument zulässig.
*/

void lfree (void *p)
{
	lrealloc(p, 0);
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

	if	(!ptr && !size)
		return NULL;

	if	(ptr)
		lmalloc_trace(ptr, 0);

	if	(realloc_function)
		p = realloc_function(ptr, size);
	else
		p = realloc_errcheck(ptr, size);

	if	(size)
		lmalloc_trace(p, size);

	return p;
}

/*
$Notes
:*:
For small memory segments, the functions |memalloc| and |memfree| should
be used instead.
:de:
Für kleine Speichersegmente sollten
die Funktionen |memalloc| und |memfree| eingesetzt
werden.

$Warnings
:*:
:de:Bei Übergabe eines Pointers an die Funktion
|lfree|, der nicht durch einen vorangegangenen Aufruf von
|lmalloc| stammt, ist das Verhalten unbestimmt und kann
zu einem Programmabsturz führen.
Die Funktionen |memalloc| und |memfree| sind diesbezüglich robust.
$SeeAlso
\mref{alloctab(3)}, \mref{memalloc(3)}.\br
\mref{malloc(3S)} @PRM.
*/

