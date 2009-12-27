/*
Datenbank sortieren, Eintrag suchen

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/vecbuf.h>
#include <EFEU/database.h>


/*	Vergleichsfunktion für Dateneinträge
*/

static EfiFunc *DB_cmpfunc = NULL;

static int DB_cmp(const void *a, const void *b)
{
	int n;

	CallFunc(&Type_int, &n, DB_cmpfunc, a, b);
	return n;
}


/*	Eintrag suchen/ersetzen/löschen
*/

void *DB_search(EfiDB *db, void *data, int flag)
{
	EfiFunc *save;

	save = DB_cmpfunc;
	DB_cmpfunc = db->cmp;
	data = vb_search(&db->buf, data, db->cmp ? DB_cmp : NULL, flag);
	DB_cmpfunc = save;
	return data;
}


/*	Datenbank sortieren
*/

void DB_sort(EfiDB *db)
{
	if	(db && db->cmp)
	{
		EfiFunc *save;

		save = DB_cmpfunc;
		DB_cmpfunc = db->cmp;
		qsort(db->buf.data, db->buf.used, db->type->size, DB_cmp);
		DB_cmpfunc = save;
	}
}
