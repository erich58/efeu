/*
Datenmatrizen auswerten

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

#ifndef	EFEU_mdeval_h
#define	EFEU_mdeval_h	1

#include <EFEU/mdmat.h>

typedef struct {
	void *(*init) (void *par, EfiType *type);
	void *(*exit) (void *par);
	void *(*newidx) (const void *idx, char *name);
	void (*clridx) (void *idx);
	void (*start) (void *par, const void *idx);
	void (*end) (void *par);
	void (*data) (void *par, EfiType *type, void *ptr, void *base);
} MdEvalDef;

void *md_eval (MdEvalDef *eval, void *par, mdmat *md,
	unsigned mask, unsigned base, int lag);

#endif	/* EFEU/mdeval.h */
