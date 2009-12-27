/*
:*:	interpreter status
:de:	Interpreterstatus

$Header	<EFEU/$1>

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

#ifndef	EFEU_EfiStat_h
#define	EFEU_EfiStat_h	1

#include <EFEU/object.h>
#include <EFEU/stack.h>

/*
$Notes
:*:The EfiStat concept is still in work. It should allow to work with
dfferent instances of the EFEU-interpreter. But currently the most
status parameters are still stored in global variables.
*/

typedef struct EfiStat {
	REFVAR;	
	char *name;
	EfiSrc *src;
	Stack *src_stack;
} EfiStat;

extern RefType EfiStat_reftype;
Efi *EfiStat_alloc (const char *name);

EfiSrc *EfiSrc_alloc (const char *fmt, ...);
void EfiSrc_push (EfiStat *efi, EfiSrc *src);

/*
$SeeAlso
\mref{EfiStat(3)},
*/

#endif	/* EFEU/EfiStat.h */
