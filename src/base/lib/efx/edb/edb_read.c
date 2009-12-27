/*
Einlesen eines Datensatzes

$Copyright (C) 2006 Erich Frühstück
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

#include <EFEU/EDB.h>

void edb_closein (EDB *edb)
{
	if	(edb && edb->read)
	{
		rd_debug(edb, "close input");
		rd_deref(edb->ipar);
		edb->save = 0;
		edb->ipar = NULL;
		edb->read = NULL;
	}
}

int edb_read (EDB *edb)
{
	int rval;

	if	(!edb || !edb->read)
		return 0;

	if	(edb->save)
	{
		edb->save = 0;
		return 1;
	}
	
	rval = edb->read(edb->obj->type, edb->obj->data, edb->ipar);

	if	(rval == 0)
	{
		CleanData(edb->obj->type, edb->obj->data);
		edb_closein(edb);
	}

	return rval;
}

void edb_unread (EDB *edb)
{
	if	(edb && edb->read)
		edb->save = 1;
}
