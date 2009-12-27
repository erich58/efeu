/*
:*:	Makro to construct administration functions
:de:	Makro zur Einrichtung einer Administrationsfunktion

$Header	<EFEU/$1>

$Copyright (C) 1999 Erich Frühstück
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

#ifndef	EFEU_admin_h
#define	EFEU_admin_h	1

/*
$Description
:de:
Eine Administrationsfunktion erlaubt das Erzeugen, Freigeben, Löschen und
Kopieren eines Datentypes.

---- verbatim
create	x = admin(NULL, NULL)	ALLOC
copy	x = admin(NULL, y)	ALLOC + COPY

delete	NULL = admin(x, NULL)	CLEAN + FREE
clean	x = admin(x, x)		CLEAN
assign	x = admin(x, y)		CLEAN + COPY
----

$Error
:*:
The construction of administration functions seems to complicated.
A structure with different components
for the single tasks is more handy. So this concept may be removed in
future.
:de:
Die Verwaltung von Datenweten über eine Administrationsfunktion hat sich
als zu unhandlich erwiesen. Eine Struktur mit einzelnen Komponenten
für jede Aufgabe scheint die bessere Lösung zu sein. Dieses Konzept
wird möglicherwweise in späteren Versionen verschwinden.
*/

#include <EFEU/types.h>

/*
:de:
Der Makro |$1| dient zur Konstruktion einer Administrationsfunktion.
*/

#define	ADMIN(type, name, alloc, copy, clean, free)	\
type *name (type *tg, const type *src)	\
{					\
	if	(tg)			\
	{				\
		clean			\
					\
		if	(src == NULL)	\
		{			\
			free		\
			return NULL;	\
		}			\
					\
		if	(src == tg)	\
			return tg;	\
	}				\
	else				\
	{				\
		alloc			\
					\
		if	(src == NULL)	\
			return tg;	\
	}				\
					\
	copy				\
	return tg;			\
}

#if	0	/* Beispiele */
ADMIN(TYPE, NAME,
	/* ALLOC */
	tg = memalloc(sizeof(int));
,
	/* COPY */
	memcpy(tg, src, sizeof(TYPE)),
,
	/* CLEAN */
	memset(tg, 0, sizeof(TYPE)),
,
	/* FREE */
	memfree(tg);
)

ADMIN(char, str_admin,
	/* ALLOC */
,
	/* COPY */
	tg = mstrcpy(src);
,
	/* CLEAN */
	memfree(tg);
	tg = NULL;
,
	/* FREE */
)

ADMIN(int, int_admin,
	/* ALLOC */
	tg = memalloc(sizeof(int));
,
	/* COPY */
	*tg = *src;
,
	/* CLEAN */
	*tg = 0;
,
	/* FREE */
	memfree(tg);
)

#endif

/*
$SeeAlso
\mref{types(5)}.\br
*/

#endif /* EFEU/admin.h	*/
