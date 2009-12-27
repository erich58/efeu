/*
Ausgabedefinitionen

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

#ifndef	EFEU_PRINTOBJ_H
#define	EFEU_PRINTOBJ_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>


Func_t *GetPrintFunc (const Type_t *type);
int PrintData (io_t *io, const Type_t *type, const void *data);
int PrintVecData (io_t *io, const Type_t *type, const void *data, size_t dim);

int PrintObj (io_t *io, const Obj_t *obj);
int PrintFmtList (io_t *io, const char *fmt, ObjList_t *list);
int PrintFmtObj (io_t *io, const char *fmt, const Obj_t *obj);
int PrintAny (io_t *io, const Type_t *type, const void *data);

extern char *PrintListBegin;
extern char *PrintListDelim;
extern char *PrintListEnd;

#endif	/* EFEU_PRINTOBJ_H */
