/*
Ausgeben eines Datensatzes

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

#define	FMT_NWRITE	"[edb:nwrite]$0: $1 records written.\n"

void edb_closeout (EDB *edb)
{
	if	(edb && edb->write)
	{
		char *p = edb->nwrite ? rd_ident(edb->opar) : 0;
		rd_debug(edb, "close output");

		if	(p)
		{
			dbg_message("edb", DBG_STAT, FMT_NWRITE,
				p, "u", (unsigned) edb->nwrite);
		}

		rd_deref(edb->opar);
		edb->opar = NULL;
		edb->write = NULL;
		edb->nwrite = 0;
	}
}

void edb_write (EDB *edb)
{
	if	(!edb || !edb->write)
		return;

	if	(!edb->write(edb->obj->type, edb->obj->data, edb->opar))
	{
		edb_closeout(edb);
	}
	else	edb->nwrite++;
}
