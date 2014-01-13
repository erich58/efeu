/*	Datensätze duplizieren

$Copyright (C) 2004 Erich Frühstück
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

/*
Verdopplungsliste
*/

typedef struct DLISTStruct {
	struct DLISTStruct *next;
	EfiStruct *var;
	EfiObj *obj;
} DLIST;

static DLIST *dlist (EfiStruct *var, const char *expr)
{
	DLIST *t = memalloc(sizeof *t);
	t->next = NULL;
	t->var = var;

	if	(expr && IsTypeClass(var->type, &Type_enum))
	{
		int key = NextEnumCode(var->type, 0);
		AddEnumKey(var->type, mstrcpy(expr), NULL, key);
		t->obj = ConstObj(var->type, &key);
	}
	else if	(expr)
	{
		t->obj = EvalObj(strterm(expr), var->type);
	}
	else	t->obj = ConstObj(var->type, NULL);

	return t;
}

static void dlist_free (DLIST *dlist)
{
	if	(dlist)
	{
		dlist_free(dlist->next);
		UnrefObj(dlist->obj);
		memfree(dlist);
	}
}

/*
Parameterstruktur
*/

typedef struct {
	REFVAR;
	EDB *base;
	DLIST *list;
	DLIST *ptr;
	int rval;
} DUP;

static void dup_clean (void *data)
{
	DUP *dup = data;
	rd_deref(dup->base);
	dlist_free(dup->list);
	memfree(dup);
}

static RefType dup_reftype = REFTYPE_INIT("EDB_DUP", NULL, dup_clean);

static void *dup_alloc (EDB *edb, const char *vlist)
{
	DUP *dup;
	char *expr;
	DLIST **ptr;
	EfiStruct *var;
	char **list;
	size_t dim;
	int i;
	
	dup = memalloc(sizeof *dup);
	dup->base = edb;
	dup->list = NULL;
	dup->ptr = NULL;
	dup->rval = 0;
	ptr = &dup->list;
	dim = mstrsplit(vlist, "%s,", &list);

	for (i = 0; i < dim; i++)
	{
		if	((expr = strchr(list[i], '=')))
			*expr++ = 0;

		for (var = edb->obj->type->list; var != NULL; var = var->next)
		{
			if	(mstrcmp(var->name, list[i]) == 0)
			{
				*ptr = dlist(var, expr);
				ptr = &(*ptr)->next;
				break;
			}
		}
	}

	memfree(list);
	return rd_init(&dup_reftype, dup);
}

static int dup_read (EfiType *type, void *data, void *par)
{
	DUP *dup = par;

	if	(dup->ptr)
	{
		CopyData(type, data, dup->base->obj->data);
		CopyData(dup->ptr->var->type,
			(char *) data + dup->ptr->var->offset,
			dup->ptr->obj->data);
		dup->ptr = dup->ptr->next;
		return dup->rval;
	}
	else if	(edb_read(dup->base))
	{
		CopyData(type, data, dup->base->obj->data);
		dup->ptr = dup->list;
		dup->rval = 1;
		return dup->rval;
	}
	else	return 0;
}

static EDB *fdef_dup (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	EDB *edb = edb_create(base->obj->type);
	edb->read = dup_read;
	edb->ipar = dup_alloc(base, arg);
	return edb;
}

EDBFilter EDBFilter_dup = EDB_FILTER(NULL,
	"dup", "=list", fdef_dup, NULL,
	":*:duplicate records"
	":de:Datensätze duplizieren"
);
