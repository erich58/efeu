/*
Standardtypen

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

#ifndef	EFEU_stdtype_h
#define	EFEU_stdtype_h	1

#include <EFEU/object.h>

void Clean_obj (const EfiType *st, void *data);
void Clean_ref (const EfiType *st, void *data);
void Clean_str (const EfiType *st, void *data);

void Copy_obj (const EfiType *st, void *tg, const void *src);
void Copy_ref (const EfiType *st, void *tg, const void *src);
void Copy_str (const EfiType *st, void *tg, const void *src);

size_t Read_str (const EfiType *st, void *data, IO *io);
size_t Write_str (const EfiType *st, const void *data, IO *io);

#define	COMPLEX_TYPE(name, size, recl, read, write, base, eval, clean, copy) \
{ name, size, recl, read, write, base, eval, clean, copy, \
	0, NULL, NULL, NULL, NULL, VB_DATA(8, sizeof(EfiFunc *)), NULL, NULL }

#define	DUMMY_TYPE(name) \
COMPLEX_TYPE(name, 0, 0, NULL, NULL, \
	NULL, NULL, NULL, NULL);

#define	EVAL_TYPE(name, type, eval, clean, copy) \
COMPLEX_TYPE(name, sizeof(type), 0, NULL, NULL, \
	NULL, eval, clean, copy)

#define	SIMPLE_TYPE(name, type, base)	\
COMPLEX_TYPE(name, sizeof(type), sizeof(type), NULL, NULL, \
	base, NULL, NULL, NULL)

#define	STD_TYPE(name, type, base, clean, copy)	\
COMPLEX_TYPE(name, sizeof(type), 0, NULL, NULL, \
	base, NULL, clean, copy)

#define	REF_TYPE(name, type)	\
COMPLEX_TYPE(name, sizeof(type), 0, NULL, NULL, \
	&Type_ref, NULL, Clean_ref, Copy_ref)

#define	IOREF_TYPE(name, type, read, write) \
COMPLEX_TYPE(name, sizeof(type), 0, read, write, \
	&Type_ref, NULL, Clean_ref, Copy_ref)

#endif	/* EFEU/stdtype.h */
