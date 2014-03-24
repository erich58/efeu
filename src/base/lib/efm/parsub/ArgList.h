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
#include <EFEU/StrPool.h>

/*
:de:
Die Struktur |$1| verwaltet einen Vektor von Strings. Sie baut auf
einen Stringpool auf.
*/

typedef struct {
	STRPOOL_VAR;
	size_t size;
	size_t dim;
	size_t *index;
} ArgList;

ArgList *arg_create (void);
StrPool *arg_next (ArgList *args);
void arg_undo (ArgList *args);
void arg_printf (ArgList *args, const char *fmt, ...);
void arg_append (ArgList *args, const char *fmt, va_list list);
void arg_cadd (ArgList *args, const char *arg);
void arg_madd (ArgList *args, char *arg);
void arg_cset (ArgList *args, int n, const char *val);
void arg_mset (ArgList *args, int n, char *val);
char *arg_get (ArgList *args, int n);
char *arg_xget (ArgList *args, int n);
int arg_test (ArgList *args, int n);

/*
$SeeAlso
\mref{ArgList(3)}.
*/

#endif	/* EFEU/ArgList.h */
