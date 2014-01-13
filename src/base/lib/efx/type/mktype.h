/*
Datentypkonstruktion

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_mktype_h
#define	EFEU_mktype_h	1

#include <EFEU/stdtype.h>

typedef struct {
	const char *type;
	const char *name;
	size_t width;
	void (*update) (void *data, void *base);
	void (*sync) (void *data, void *base);
	const char *desc;
} EfiBitfield;

extern EfiLval Lval_bitfield;

EfiStruct *EfiType_bitfield (EfiType *base, EfiStruct *last,
	size_t offset, EfiBitfield *bf);

EfiStruct *EfiType_addvar (EfiType *base, EfiStruct *last,
	EfiStruct *var);

EfiStruct *EfiType_stdvar (EfiType *base, EfiStruct *last,
	size_t offset, size_t dim,
	const char *vtype, const char *vname, const char *desc);

#endif	/* EFEU/mktype.h */
