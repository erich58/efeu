/*
Datenbank

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

#ifndef	EFEU_database_h
#define	EFEU_database_h	1

#include <EFEU/object.h>
#include <EFEU/vecbuf.h>
#include <EFEU/refdata.h>


/*	Datenbankstruktur
*/

typedef struct {
	REFVAR;		/* Referenzzähler */
	EfiType *type;	/* Datentype */
	EfiFunc *cmp;	/* Vergleichsfunktion */
	VecBuf buf;	/* Datenbuffer */
} EfiDB;

EfiDB *DB_create (EfiType *type, size_t blk);

void DB_load (IO *io, EfiDB *db, EfiObj *expr);
void DB_xload (IO *io, EfiDB *db, EfiObj *expr);

void DB_save (IO *io, EfiDB *db, int mode, EfiVirFunc *test, char *list);

void *DB_search (EfiDB *db, void *data, int flag);
void DB_sort (EfiDB *db);

extern EfiType Type_DB;

void SetupDataBase (void);

#endif	/* EFEU/database.h */
