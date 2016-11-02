/*	Aggregationsfilter

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

#define	FMT_NOADD	"$!: don't know how to add data of type \"$1\".\n"
#define	FMT_NOCMP	"$!: don't know how to compare data of type \"$1\".\n"

/*
Testliste
*/

typedef struct TLISTStruct {
	struct TLISTStruct *next;
	size_t offset;
	int (*test) (EfiFunc *func, const void *a, const void *b);
	EfiFunc *func;
} TLIST;

static int test_enum (EfiFunc *func, const void *a, const void *b)
{
	return Val_int(a) != Val_int(b);
}

static TLIST *tlist_enum (size_t offset)
{
	TLIST *t = memalloc(sizeof *t);
	t->next = NULL;
	t->offset = offset;
	t->test = test_enum;
	t->func = NULL;
	return t;
}

static int test_func (EfiFunc *func, const void *a, const void *b)
{
	int rval = 1;
	CallFunc(&Type_bool, &rval, func, a, b);
	return !rval;
}

static TLIST *tlist (EfiType *type, size_t offset, size_t dim)
{
	TLIST *t = memalloc(sizeof *t);
	t->next = NULL;
	t->offset = offset;
	t->test = test_func;
	t->func = GetFunc(&Type_bool, GetGlobalFunc("=="),
		2, type, 1, type, 0); 

	if	(!t->func)
		log_note(edb_note, FMT_NOCMP, "s", type->name);

	return t;
}


static void tlist_free (TLIST *tlist)
{
	if	(tlist)
	{
		tlist_free(tlist->next);
		rd_deref(tlist->func);
		memfree(tlist);
	}
}

/*
Additionsliste
*/

typedef struct SLISTStruct {
	struct SLISTStruct *next;
	size_t offset;
	EfiFunc *add;
} SLIST;

static SLIST *slist (EfiType *type, size_t offset, size_t dim)
{
	SLIST *s = memalloc(sizeof *s);
	s->next = NULL;
	s->offset = offset;
	s->add = GetFunc((EfiType *) type, GetTypeFunc(type, "+="),
		2, type, 1, type, 0); 

	if	(!s->add)
		log_note(edb_note, FMT_NOADD, "s", type->name);

	return s;
}

static void slist_free (SLIST *slist)
{
	if	(slist)
	{
		slist_free(slist->next);
		rd_deref(slist->add);
		memfree(slist);
	}
}

/*
Parameterstruktur
*/

typedef struct {
	REFVAR;
	EDB *base;
	TLIST *tlist;
	SLIST *slist;
} SUM;

static void sum_clean (void *data)
{
	SUM *sum = data;
	rd_deref(sum->base);
	fprintf(stderr, "sum_clean\n");
	tlist_free(sum->tlist);
	slist_free(sum->slist);
	memfree(sum);
}

static RefType sum_reftype = REFTYPE_INIT("EDB_SUM", NULL, sum_clean);

static void *sum_alloc (EDB *edb, const char *vlist)
{
	SUM *sum;
	TLIST **tp;
	SLIST **sp;
	EfiStruct *var;
	char **list;
	size_t dim;
	
	sum = memalloc(sizeof *sum);
	sum->base = edb;
	sum->tlist = NULL;
	tp = &sum->tlist;
	sum->slist = NULL;
	sp = &sum->slist;

	dim = mstrsplit(vlist, "%s,", &list);

	for (var = edb->obj->type->list; var != NULL; var = var->next)
	{
		if	(IsTypeClass(var->type, &Type_enum))
		{
			*tp = tlist_enum(var->offset);
			tp = &(*tp)->next;
		}
		else if	(patselect(var->name, list, dim))
		{
			*sp = slist(var->type, var->offset, var->dim);
			sp = &(*sp)->next;
		}
		else
		{
			*tp = tlist(var->type, var->offset, var->dim);
			tp = &(*tp)->next;
		}
	}

	if	(!sum->slist && !sum->tlist)
		sum->slist = slist(edb->obj->type, 0, 0);

	memfree(list);
	return rd_init(&sum_reftype, sum);
}

static int sum_test (SUM *spar, const char *a, const char *b)
{
	TLIST *t;

	for (t = spar->tlist; t != NULL; t = t->next)
		if (t->test(t->func, a + t->offset, b + t->offset))
			return 1;

	return 0;
}

static void sum_add (SUM *spar, const char *a, const char *b)
{
	SLIST *s;

	for (s = spar->slist; s != NULL; s = s->next)
		CallVoidFunc(s->add, a + s->offset, b + s->offset);
}

static int sum_read (EfiType *type, void *data, void *par)
{
	SUM *sum = par;

	if	(!edb_read(sum->base))
		return 0;

	CopyData(type, data, sum->base->obj->data);

	while ((edb_read(sum->base)))
	{
		if	(sum_test(sum, data, sum->base->obj->data))
			break;

		sum_add(sum, data, sum->base->obj->data);
	}

	edb_unread(sum->base);
	return 1;
}

static EDB *fdef_sum (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	EDB *edb = edb_create(base->obj->type);
	edb->desc = mstrcpy(base->desc);
	edb->read = sum_read;
	edb->ipar = sum_alloc(base, arg);
	return edb;
}

EDBFilter EDBFilter_sum = EDB_FILTER(NULL,
	"sum", "=list", fdef_sum, NULL,
	":*:summing records"
	":de:Aufsummieren von Datensätzen"
);
