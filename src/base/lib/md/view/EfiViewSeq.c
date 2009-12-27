/*
Datentyp für Ablauffolge

$Copyright (C) 2007 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/EfiView.h>
#include <EFEU/stdtype.h>

EfiType Type_EfiViewSeq = COMPLEX_TYPE("EfiViewSeq", "EfiViewSeq",
	sizeof(EfiViewSeq), 0, Struct_read, Struct_write, NULL,
	NULL, NULL, NULL, Memory_copy, 0);

void EfiViewSeq_setup (void)
{
	static int setup_done = 0;
	EfiType *type;
	EfiStruct *st_var;

	if	(setup_done)	return;

	setup_done = 1;

	SetupStd();
	type = &Type_EfiViewSeq;
	AddType(type);
	st_var = type->list;

	st_var = EfiType_stdvar(type, st_var,
		offsetof(EfiViewSeq, id), 0, "varsize", "id",
		":*:serial number:de:Laufnummer");
	st_var = EfiType_stdvar(type, st_var,
		offsetof(EfiViewSeq, rec), 0, "varsize", "rec",
		":*:record number:de:Datensatznummer");
	st_var = EfiType_stdvar(type, st_var,
		offsetof(EfiViewSeq, num), 0, "varsize", "num",
		":*:event number:de:Ereignisnummer");
}
