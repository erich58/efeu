/*
:*:	compare definition
:de:	Vergleichsdefinition

$Header <EFEU/$1>

$Copyright (C) 2005 Erich Frühstück
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

#ifndef	EFEU_CmpDef_h
#define	EFEU_CmpDef_h	1

#include <EFEU/object.h>

typedef struct CmpDefEntryStruct CmpDefEntry;

struct CmpDefEntryStruct {
	CmpDefEntry *next;
	EfiType *type;
	int invert;
	size_t dim;
	size_t offset;
	int (*cmp)(CmpDefEntry *entry, void *a, void *b);
	void (*clean)(void *par);
	void *par;
};

CmpDefEntry *cmp_member (EfiStruct *st, int inv);

typedef struct {
	REFVAR;
	EfiType *type;
	CmpDefEntry *list;
} CmpDef;

CmpDef *cmp_alloc (EfiType *type, CmpDefEntry *list);
CmpDef *cmp_create (EfiType *type, const char *def, char **ptr);
int cmp_data (CmpDef *cmp, const void *a, const void *b);

void SetupCmpDef(void);

#endif	/* EFEU/CmpDef.h */
