/*
Standardparameter zur EDB-Datenbankverknüpfung

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

#include <EFEU/EDBJoin.h>

static void jp_clean (void *opaque_data)
{
	EDBJoinPar *par = opaque_data;
	rd_deref(par->db1);
	rd_deref(par->db2);
	memfree(par);
}

static RefType jp_reftype = REFTYPE_INIT("EDBJoinPar", NULL, jp_clean);

EDBJoinPar *EDBJoinPar_create (EDB *db1, EDB *db2)
{
	EDBJoinPar *par = memalloc(sizeof *par);
	par->db1 = db1;
	par->db2 = db2;
	return rd_init(&jp_reftype, par);
}
