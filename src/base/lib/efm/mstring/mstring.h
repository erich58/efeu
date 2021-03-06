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

#ifndef	EFEU_mstring_h
#define	EFEU_mstring_h	1

#include <EFEU/memalloc.h>
#include <EFEU/unicode.h>
#include <EFEU/stdint.h>
#include <EFEU/io.h>

size_t mstrlen (const char *a);
int mstrcmp (const char *a, const char *b);
char *mstrchr (const char *a, const char *b);
int mtestkey (const char *str, char **ptr, const char *key);

char *mstrcat (const char *delim, const char *str1, ...);
char *mstrcpy (const char *str);
char *mstrncpy (const char *str, size_t len);
char *mstrpaste (const char *delim, const char *a, const char * b);
char *mtabcat (const char *delim, char **list, size_t dim);

char *mvsprintf (const char *fmt, va_list list);
char *msprintf (const char *fmt, ...);

size_t mstrsplit (const char *str, const char *delim, char ***ptr);
char *nextstr (char **ptr);

char *mstrcut (const char *str, char **ptr, char *delim, int flag);
char *mlangcpy (const char *fmt, const char *lang);

char *mdirname (const char *path, int flag);
char *mbasename (const char *path, char **suffix);

int64_t mstr2int64 (const char *str, char **ptr, int base);
uint64_t mstr2uint64 (const char *str, char **ptr, int base);
int32_t mstr2ucs (const char *str, char **ptr);

void mtrim (char *s);

#endif	/* EFEU/mstring.h */
