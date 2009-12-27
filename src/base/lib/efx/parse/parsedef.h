/*
Parse-Definitionen

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

#ifndef	EFEU_parsedef_h
#define	EFEU_parsedef_h	1

#include <EFEU/object.h>

typedef struct {
	char *name;
	EfiObj *(*func) (IO *io, void *data);
	void *data;
} EfiParseDef;

EfiParseDef *NewParse (const EfiType *type, const char *name, size_t dim);
void DelParse (EfiParseDef *var);

void *NewParseTab (size_t dim);
void DelParseTab (void *tab);
void PushParseTab (void *tab);
void *PopParseTab (void);

void AddParseDef (EfiParseDef *pdef, size_t dim);
EfiParseDef *GetParseDef (const char *name);
void ListParseDef (IO *io);

EfiObj *PFunc_typedef (IO *io, void *data);
EfiObj *PFunc_struct (IO *io, void *data);
EfiObj *PFunc_construct (IO *io, void *data);
EfiObj *PFunc_enum (IO *io, void *data);

EfiObj *PFunc_bool (IO *io, void *data);
EfiObj *PFunc_int (IO *io, void *data);
EfiObj *PFunc_str (IO *io, void *data);
EfiObj *PFunc_type (IO *io, void *data);

EfiObj *PFunc_for (IO *io, void *data);
EfiObj *PFunc_while (IO *io, void *data);
EfiObj *PFunc_do (IO *io, void *data);
EfiObj *PFunc_if (IO *io, void *data);
EfiObj *PFunc_break (IO *io, void *data);
EfiObj *PFunc_return (IO *io, void *data);
EfiObj *PFunc_switch (IO *io, void *data);
EfiObj *PFunc_case (IO *io, void *data);
EfiObj *PFunc_default (IO *io, void *data);

#endif	/* EFEU/parsedef.h */
