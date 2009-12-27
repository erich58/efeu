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

typedef Obj_t *(*ParseFunc_t) (io_t *io, void *data);

typedef struct {
	char *name;
	ParseFunc_t func;
	void *data;
} ParseDef_t;

ParseDef_t *NewParse (const Type_t *type, const char *name, size_t dim);
void DelParse (ParseDef_t *var);

void *NewParseTab (size_t dim);
void DelParseTab (void *tab);
void PushParseTab (void *tab);
void *PopParseTab (void);

void AddParseDef (ParseDef_t *pdef, size_t dim);
ParseDef_t *GetParseDef (const char *name);
void ListParseDef (io_t *io);

Obj_t *PFunc_typedef (io_t *io, void *data);
Obj_t *PFunc_struct (io_t *io, void *data);
Obj_t *PFunc_enum (io_t *io, void *data);

Obj_t *PFunc_bool (io_t *io, void *data);
Obj_t *PFunc_int (io_t *io, void *data);
Obj_t *PFunc_str (io_t *io, void *data);
Obj_t *PFunc_type (io_t *io, void *data);

Obj_t *PFunc_for (io_t *io, void *data);
Obj_t *PFunc_while (io_t *io, void *data);
Obj_t *PFunc_do (io_t *io, void *data);
Obj_t *PFunc_if (io_t *io, void *data);
Obj_t *PFunc_break (io_t *io, void *data);
Obj_t *PFunc_return (io_t *io, void *data);
Obj_t *PFunc_switch (io_t *io, void *data);
Obj_t *PFunc_case (io_t *io, void *data);
Obj_t *PFunc_default (io_t *io, void *data);

#endif	/* EFEU/parsedef.h */
