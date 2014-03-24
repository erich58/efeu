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

#ifndef	EFEU_printobj_h
#define	EFEU_printobj_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>


EfiFunc *GetPrintFunc (const EfiType *type);
int PrintData (IO *io, const EfiType *type, const void *data);
int PrintVecData (IO *io, const EfiType *type, const void *data, size_t dim);
int ShowData (IO *io, const EfiType *type, const void *data);
int ShowVecData (IO *io, const EfiType *type, const void *data, size_t dim);

int ShowObj (IO *io, const EfiObj *obj);
int PrintObj (IO *io, const EfiObj *obj);
int PrintFmtList (IO *io, const char *fmt, EfiObjList *list);
int PrintFmtObj (IO *io, const char *fmt, const EfiObj *obj);
int ShowAny (IO *io, const EfiType *type, const void *data);
int StrBufFmtList (StrBuf *sb, const char *fmt, EfiObjList *list);

int PrintType (IO *io, const EfiType *type, int verbosity);
void TypeInfo (const EfiType *type, const char *mode);

extern char *PrintListBegin;
extern char *PrintListDelim;
extern char *PrintListEnd;
extern int PrintFieldWidth;

void SetFloatPrec (const char *def);
int PrintDouble (IO *io, double val);
char *DoubleToString (double val);

#endif	/* EFEU/printobj.h */
