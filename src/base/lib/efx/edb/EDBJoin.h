/*
EDB-Datenbankverknüpfung

$Copyright (C) 2007 Erich Frühstück
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

#ifndef	EFEU_EDBJoin_h
#define	EFEU_EDBJoin_h	1

#include <EFEU/EDB.h>

/*
Verknüpfungsdefinition
*/

typedef struct EDBJoin EDBJoin;

struct EDBJoin {
	EfiType *t1;
	EfiType *t2;
	EfiType *type;
	EDB *(*create) (EDBJoin *def, EDB *db1, EDB *db2, const char *arg);
	char *par;
	char *desc;
};

void AddEDBJoin (EDBJoin *def);
EDBJoin *GetEDBJoin (EfiType *t1, EfiType *t2);
void EDBJoinInfo (InfoNode *info);

/*
Standardparameter
*/

typedef struct {
	REFVAR;		/* Referenzvariablen */
	EDB *db1;	/* 1. Datenbank */
	EDB *db2;	/* 2. Datenbank */
} EDBJoinPar;

EDBJoinPar *EDBJoinPar_create (EDB *db1, EDB *db2);

#endif	/* EFEU/EDBJoin.h */
