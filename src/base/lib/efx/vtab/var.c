/*
Variablentabellen

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

#include <EFEU/object.h>
#include <EFEU/Debug.h>

#define	TRACE	0

#if	TRACE
static LogControl trace_ctrl = LOG_CONTROL("NewStruct", LOGLEVEL_TRACE);
#endif

static ALLOCTAB(deftab, "EfiStruct", 0, sizeof(EfiStruct));

static char *var_ident (const void *data)
{
	const EfiStruct *var = data;
	return mstrpaste(" ", var->type->name, var->name);
}

static void var_clean (void *data)
{
	EfiStruct *var = data;

	memfree(var->name);
	memfree(var->desc);
	UnrefObj(var->defval);

	if	(var->clean)
		var->clean(var);

#if	TRACE
	log_note(&trace_ctrl, "DelStruct[$2]: $1\n", "pz", var, deftab.nused);
#endif
	del_data(&deftab, var);
}

static RefType var_reftype = REFTYPE_INIT("EfiStruct", var_ident, var_clean);

EfiStruct *NewEfiStruct (EfiType *type, const char *name, size_t dim)
{
	EfiStruct *var;

	var = new_data(&deftab);
	var->reftype = NULL;
	var->refcount = 0;
#if	TRACE
	log_note(&trace_ctrl, "NewStruct[$2]: $1\n", "pz", var, deftab.nused);
#endif
	memset(var, 0, sizeof(EfiStruct));
	var->name = mstrcpy(name);
	var->type = type;
	var->dim = dim;
	return rd_init(&var_reftype, var);
}

EfiStruct *RefEfiStruct (const EfiStruct *var)
{
	const EfiStruct *p;

	for (p = var; p; p = p->next)
		rd_refer(p);

	return (EfiStruct *) var;
}

void DelEfiStruct (EfiStruct *var)
{
	if	(var)
	{
		DelEfiStruct(var->next);
		rd_deref(var);
	}
}
