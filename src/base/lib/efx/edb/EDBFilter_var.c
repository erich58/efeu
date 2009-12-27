/*	Variablen neu zusammenstellen

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

typedef struct {
	REFVAR;
	EDB *base;
	EfiFunc *func;
} COMPOSE;

static void var_clean (void *data)
{
	COMPOSE *par = data;
	rd_deref(par->base);
	rd_deref(par->func);
	memfree(par);
}

static RefType var_reftype = REFTYPE_INIT("EDB_COMPOSE", NULL, var_clean);

static int var_read (EfiType *type, void *data, void *opaque_par)
{
	COMPOSE *par = opaque_par;

	if	(edb_read(par->base))
	{
		par->func->eval(par->func, data, &par->base->obj->data);
		return 1;
	}

	return 0;
}

static int can_reduce (EfiType *type)
{
	if	(!type->list)		return 0;
	if	(type->list->next)	return 0;
	if	(type->list->dim)	return 0;

	return 1;
}

static EDB *fdef_var (EDBFilter *filter, EDB *edb,
	const char *opt, const char *arg)
{
	EfiFunc *func;
	EfiType *type;
	COMPOSE *par;
	
	func = ConstructFunc(edb->obj->type, NULL, arg);

	if	(!func)
		return NULL;

	par = memalloc(sizeof *par);
	par->base = edb;
	par->func = rd_refer(func);

	type = func->type;

	if	(opt && *opt == 'r' && can_reduce(type))
		type = type->list->type;

	edb = edb_create(LvalObj(NULL, type), NULL);
	edb->read = var_read;
	edb->ipar = rd_init(&var_reftype, par);
	return edb;
}

EDBFilter EDBFilter_var = {
	"var", "[r]=list", fdef_var, NULL,
	":*:rearrange variables with expresions and patterns"
	":de:Neuzusammenstellung von Variablen über Ausdrücke und Muster"
};
