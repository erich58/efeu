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
#include <EFEU/EDBFilter.h>
#include <EFEU/konvobj.h>

#define	ERR_CONV "[edb:conv]don't know how to convert $1 in $2.\n"
#define	ERR_TYP	"[edb:typ]$!: unknown datatype $1.\n"

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
	
	if	(!arg || !*arg)
	{
		EfiVar_list(ioerr, NULL, edb->obj->type->vtab);
		exit(EXIT_SUCCESS);
	}

	func = ConstructObjFunc(NULL, arg, edb->obj);

	if	(!func)
		return NULL;

	par = memalloc(sizeof *par);
	par->base = edb;
	par->func = func;

	type = func->type;

	if	(can_reduce(type))
	{
		if	((opt && *opt == 'r') ||
			mstrcmp("this", type->list->name) == 0)
			type = type->list->type;
	}

	edb = edb_create(type);
	edb->desc = mstrcpy(par->base->desc);
	edb->read = var_read;
	edb->ipar = rd_init(&var_reftype, par);
	return edb;
}

EDBFilter EDBFilter_var = EDB_FILTER(NULL,
	"var", "[r]=list", fdef_var, NULL,
	":*:rearrange variables with expresions and patterns"
	":de:Neuzusammenstellung von Variablen über Ausdrücke und Muster"
);

EDB *edb_conv (EDB *edb, EfiType *type)
{
	EfiKonv *conv;
	COMPOSE *par;

	if	(!edb || !type || edb->obj->type == type)
		return edb;

	conv = GetKonv(NULL, edb->obj->type, type);

	if	(conv && conv->func)
	{
		par = memalloc(sizeof *par);
		par->base = edb;
		par->func = rd_refer(conv->func);

		edb = edb_create(type);
		edb->desc = mstrcpy(par->base->desc);
		edb->read = var_read;
		edb->ipar = rd_init(&var_reftype, par);
		return edb;
	}
	else
	{
		dbg_note(NULL, ERR_CONV, "mm", edb->obj->type->name,
			type->name);

		rd_deref(edb);
		return NULL;
	}
}

EDB *edb_xconv (EDB *edb, const char *name)
{
	EfiType *type = XGetType(name);

	if	(type)
		return edb_conv(edb, type);
	
	dbg_error("edb", ERR_TYP, "s", name);
	rd_deref(edb);
	return NULL;
}

static EDB *fdef_conv (EDBFilter *filter, EDB *edb,
	const char *opt, const char *arg)
{
	return edb_xconv(edb, arg);
}

EDBFilter EDBFilter_conv = EDB_FILTER(NULL,
	"conv", "=type", fdef_conv, NULL,
	":*:convert data type"
	":de:Datentyp konvertieren"
);

