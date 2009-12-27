/*
:*:	arg lists
:de:	Argumentlisten

$Header	<EFEU/$1>

$Copyright (C) 2001 Erich Frühstück
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

#ifndef	EFEU_ArgList_h
#define	EFEU_ArgList_h	1

#include <EFEU/refdata.h>
#include <EFEU/vecbuf.h>

/*
:de:
Die Struktur |$1| verwaltet einen Vektor von dynamisch generierten
Strings.
*/

typedef struct {
	REFVAR;
	size_t size;
	size_t dim;
	char **data;
} ArgList_t;

extern char *ArgList_get (ArgList_t *args, int n);
extern void ArgList_append (ArgList_t *args, const char *fmt, va_list list);
extern void ArgList_cadd (ArgList_t *args, const char *arg);
extern void ArgList_madd (ArgList_t *args, char *arg);

extern ArgList_t *ArgList (const char *def, ...);

/*
$SeeAlso
\mref{ArgList(3)}.
*/

#endif	/* EFEU/ArgList.h */
