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

#ifndef	EFEU_DatPool_h
#define	EFEU_DatPool_h	1

#include <EFEU/extension.h>
#include <EFEU/refdata.h>

typedef struct DatPool DatPool;

extern RefType DatPool_reftype;

DatPool *DatPool_map (const char *path);
DatPool *DatPool_dyn (size_t bsize);
void DatPool_create (DatPool *pool, const char *path);
size_t DatPool_add (DatPool *pool, const void *data, size_t size);
void *DatPool_get (DatPool *pool, size_t offset);

#endif	/* EFEU/DatPool.h */
