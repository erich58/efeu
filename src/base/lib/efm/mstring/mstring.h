/*
Dynamische Stringoperationen

$Header	<EFEU/$1>

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

#ifndef	_EFEU_mstring_h
#define	_EFEU_mstring_h	1

#include <EFEU/memalloc.h>

typedef char *mstr_t;

size_t mstrlen (const char *a);
int mstrcmp (const char *a, const char *b);
char *mstrchr (const char *a, const char *b);

char *mstrcat (const char *delim, const char *str1, ...);
char *mstrcpy (const char *str);
char *mstrncpy (const char *str, size_t len);
char *mstrpaste (const char *delim, const char *a, const char * b);
char *listcat (const char *delim, char **list, size_t dim);

char *mvsprintf (const char *fmt, va_list list);
char *msprintf (const char *fmt, ...);

size_t strsplit (const char *str, const char *delim, char ***ptr);
char *nextstr (char **ptr);

char *mstrcut (const char *str, char **ptr, char *delim, int flag);

char *mlangcpy (const char *fmt, const char *lang);

/*	Sonstige Stringshilfsfunktionen
*/

void *rmemcpy(void *dest, const void *src, size_t n);

#if	REVBYTEORDER
#define	MEMCPY	rmemcpy
#else
#define	MEMCPY	memcpy
#endif

#endif	/* EFEU/mstring.h */
