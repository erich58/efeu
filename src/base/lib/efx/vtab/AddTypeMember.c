/*
Datentypkomponenten registrieren

$Copyright (C) 2006 Erich Fr�hst�ck
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

#include <EFEU/object.h>

/*
Die Funktion |$1| erweitert die Variablentabelle <tab> entsprechend
der angegebenen Parameter.
*/

void AddTypeMember (EfiType *base, size_t offset, size_t dim,
	const char *vtype, const char *vname, const char *desc)
{
	EfiType *type;
	EfiVar *var, **ptr;
	char *p;

	type = XGetType(vtype);

	if	(!type)	return;

	var = NewVar(type, vname, dim);
	var->offset = offset;
	var->desc = mlangcpy(desc, NULL);
	var->member = StructMember;

	ptr = &base->list;

	while (*ptr != NULL)
		ptr = &(*ptr)->next;

	*ptr = var;
	p = (char *) base->defval + offset;

	if	(var->dim)
	{
		CopyVecData(var->type, var->dim, p, var->data);
	}
	else	CopyData(var->type, p, var->data);

	AddVar(base->vtab, var, 1);
}
