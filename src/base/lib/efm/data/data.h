/*
Datenhilfsprogramme

$Header	<EFEU/$1>

$Copyright (C) 1998 Erich Frühstück
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

#ifndef EFEU_DATA_H
#define EFEU_DATA_H	1

#include <EFEU/mstring.h>
#include <EFEU/ftools.h>

typedef size_t (*iofunc_t) (void *ptr, void *dp, size_t r, size_t s, size_t n);

size_t xloaddata (void *ptr, void *dp, size_t recl, size_t size, size_t n);
size_t floaddata (void *ptr, void *dp, size_t recl, size_t size, size_t n);
size_t mloaddata (void *ptr, void *dp, size_t recl, size_t size, size_t n);

size_t xsavedata (void *ptr, void *dp, size_t recl, size_t size, size_t n);
size_t fsavedata (void *ptr, void *dp, size_t recl, size_t size, size_t n);
size_t msavedata (void *ptr, void *dp, size_t recl, size_t size, size_t n);

typedef size_t (*iodata_t) (iofunc_t func, void *ptr, void *dp, size_t n);
typedef void (*clrdata_t) (void *dp, size_t n);

#define	mstr_size	sizeof(mstr_t)
#define	mstr_recl	0

size_t mstr_iodata (iofunc_t func, void *ptr, mstr_t *dp, size_t n);
void mstr_clrdata (mstr_t *str, size_t n);

#endif	/* EFEU/Data.h */
