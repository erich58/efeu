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
#include <EFEU/EfiPar.h>

void Clean_obj (const EfiType *st, void *data);
void Clean_ref (const EfiType *st, void *data);
void Clean_str (const EfiType *st, void *data);
void Clean_vec (const EfiType *st, void *data);

void Copy_obj (const EfiType *st, void *tg, const void *src);
void Copy_ref (const EfiType *st, void *tg, const void *src);
void Copy_str (const EfiType *st, void *tg, const void *src);
void Copy_vec (const EfiType *st, void *tg, const void *src);

size_t Read_str (const EfiType *st, void *data, IO *io);
size_t Write_str (const EfiType *st, const void *data, IO *io);
int Print_str (const EfiType *st, const void *data, IO *io);
size_t Read_vec (const EfiType *st, void *data, IO *io);
size_t Write_vec (const EfiType *st, const void *data, IO *io);
int Print_vec (const EfiType *st, const void *data, IO *io);

#define	TYPE_CNAME(type)	#type

#define	COMPLEX_TYPE(name, cname, size, recl, read, write, print, \
	base, eval, destroy, clean, copy) \
{ name, cname, NULL, size, recl, read, write, print, \
	base, eval, destroy, clean, copy, \
	0, 0, NULL, NULL, NULL, NULL, \
	VB_DATA(8, sizeof(EfiFunc *)), VB_DATA(16, sizeof(EfiParClass *)), \
	NULL, NULL, NULL }

#define	DUMMY_TYPE(name) \
COMPLEX_TYPE(name, NULL, 0, 0, NULL, NULL, NULL, \
	NULL, NULL, NULL, NULL, NULL);

#define	EVAL_TYPE(name, type, eval, clean, copy) \
COMPLEX_TYPE(name, TYPE_CNAME(type), sizeof(type), 0, NULL, NULL, NULL, \
	NULL, eval, NULL, clean, copy)

#define	SIMPLE_TYPE(name, type, base, print)	\
COMPLEX_TYPE(name, TYPE_CNAME(type), sizeof(type), sizeof(type), \
	NULL, NULL, print, \
	base, NULL, NULL, NULL, NULL)

#define	STD_TYPE(name, type, base, clean, copy)	\
COMPLEX_TYPE(name, TYPE_CNAME(type), sizeof(type), 0, NULL, NULL, NULL, \
	base, NULL, NULL, clean, copy)

#define	REF_TYPE(name, type)	\
COMPLEX_TYPE(name, TYPE_CNAME(type), sizeof(type), 0, NULL, NULL, NULL, \
	&Type_ref, NULL, NULL, Clean_ref, Copy_ref)

#define	IOREF_TYPE(name, type, read, write) \
COMPLEX_TYPE(name, TYPE_CNAME(type), sizeof(type), 0, read, write, NULL, \
	&Type_ref, NULL, NULL, Clean_ref, Copy_ref)

#endif	/* EFEU/stdtype.h */
