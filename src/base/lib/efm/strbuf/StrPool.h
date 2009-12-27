/*
:*:string data pool
:de:Sammelbecken für Zeichenketten

$Header	<EFEU/$1>

$Copyright (C) 2007 Erich Frühstück
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

#ifndef	EFEU_StrPool_h
#define	EFEU_StrPool_h	1

#include <EFEU/extension.h>
#include <EFEU/refdata.h>

#define	STRPOOL_VAR	\
	REFVAR;		\
	int (*clean) (void *data, size_t size);	\
	char *cdata;	\
	char *mdata;	\
	size_t cpos;	\
	size_t csize;	\
	size_t used;	\
	size_t msize

typedef struct {
	STRPOOL_VAR;
} StrPool;

extern RefType StrPool_reftype;

void StrPool_init (StrPool *pool);
void StrPool_clean (StrPool *pool);

StrPool *NewStrPool (void);

StrPool *StrPool_map (const char *path);
StrPool *StrPool_alloc (size_t size);
StrPool *StrPool_ext (void *data, size_t size,
	int (*clean) (void *data, size_t size));
void StrPool_save (StrPool *pool, const char *path);
void StrPool_expand (StrPool *pool, size_t s);
size_t StrPool_offset (StrPool *pool);
size_t StrPool_add (StrPool *pool, const char *str);
size_t StrPool_xadd (StrPool *pool, const char *str);
size_t StrPool_vprintf (StrPool *pool, const char *fmt, va_list args);
size_t StrPool_printf (StrPool *pool, const char *fmt, ...);
size_t StrPool_psubarg (StrPool *pool, const char *fmt, const char *def, ...);
size_t StrPool_align (StrPool *pool, size_t size);
size_t StrPool_start (StrPool *pool);
size_t StrPool_next (StrPool *pool);
char *StrPool_get (const StrPool *pool, size_t offset);
char *StrPool_mget (const StrPool *pool, size_t offset);
size_t StrPool_copy (StrPool *tg, StrPool *src, size_t offset);
int StrPool_cmp (StrPool *p1, size_t i1, StrPool *p2, size_t i2);

#endif	/* EFEU/StrPool.h */
