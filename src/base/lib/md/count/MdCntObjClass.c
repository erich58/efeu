/*
:*:provide count classes for data object
:de:Zählklassifikationen für Datenobjekt bereitstellen

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

#include <EFEU/mdcount.h>
#include <EFEU/stdtype.h>
#include <EFEU/Debug.h>

typedef struct {
	MDCLASS_VAR;
	EfiObj *obj;
	int base;
	int mdim;
	int *map;
} ObjClass;

static ALLOCTAB(buf_class, "ObjClass", 100, sizeof(ObjClass));

static int enum_dummy (MdClass *p_cdef, const void *ptr)
{
	ObjClass *cl = (ObjClass *) p_cdef;
	UpdateLval(cl->obj);
	return Val_int(cl->obj->data) ? 0 : 1;
}

static int enum_classify (MdClass *p_cdef, const void *ptr)
{
	ObjClass *cl;
	int n;
	
	cl = (ObjClass *) p_cdef;
	UpdateLval(cl->obj);
	n = Val_int(cl->obj->data) - cl->base;
	return (n >= 0 && n < cl->mdim) ? cl->map[n] : cl->dim;
}

typedef struct {
	VarTabEntry *p;
	int val;
} IDX;

static int cmp_idx (const void *ap, const void *bp)
{
	const IDX *a = ap;
	const IDX *b = bp;

	if	(a->val < b->val)	return -1;
	if	(a->val > b->val)	return 1;

	return 0;
}

static VECBUF(label_buf, 100, sizeof(IDX));

static ObjClass *enum_class (EfiObj *obj, char *name, char *desc)
{
	ObjClass *cl;
	VarTabEntry *p;
	IDX key, *idx;
	size_t n;

	cl = new_data(&buf_class);
	cl->name = name;
	cl->desc = desc;
	cl->classify = NULL;
	cl->obj = obj;

	p = obj->type->vtab->tab.data;
	n = obj->type->vtab->tab.used;
	label_buf.used = 0;

	for (; n-- > 0; p++)
	{
		if	(p->obj && p->obj->type == obj->type)
		{
			key.p = p;
			key.val = Val_int(p->obj->data);
			vb_search(&label_buf, &key, cmp_idx, VB_ENTER);
		}
	}

	if	(!label_buf.used)
	{
		cl->dim = 1;
		cl->label = malloc(sizeof cl->label[0]);
		cl->label->name = cl->name;
		cl->label->desc = cl->desc;
		cl->base = 0;
		cl->mdim = 0;
		cl->map = NULL;
		cl->classify = enum_dummy;
		return cl;
	}

	idx = label_buf.data;
	cl->dim = label_buf.used;
	cl->base = idx[0].val;
	cl->mdim = idx[cl->dim - 1].val - cl->base + 1;
	cl->label = malloc(cl->dim  * sizeof cl->label[0]);
	cl->map = malloc(cl->mdim * sizeof cl->map[0]);

	for (n = 0; n < cl->mdim; n++)
		cl->map[n] = cl->dim;

	for (n = 0; n < cl->dim; n++)
	{
		cl->label[n].name = mstrcpy(idx[n].p->name);
		cl->label[n].desc = mstrcpy(idx[n].p->desc);
		cl->map[idx[n].val - cl->base] = n;
	}

	cl->classify = enum_classify;
	return cl;
}

void MdCntObjClass (MdCountPar *tab, EfiObj *obj, char *pfx, char *desc)
{
	EfiStruct *st;

	if	(obj->type->dim)
	{
		UnrefObj(obj);
		return;
	}

	for (st = obj->type->list; st; st = st->next)
	{
		EfiObj *sub;
		char *name;

		if	(st->dim)	continue;
		if	(st->type->dim)	continue;

		sub = Var2Obj(st, obj);
		name = mstrpaste(".", pfx, st->name);
		MdCntObjClass(tab, sub, name, mstrcpy(st->desc));
	}

	if	(obj->type->flags & TYPE_ENUM)
	{
		AddMdClass(tab, enum_class(obj, pfx, desc));
		return;
	}

	UnrefObj(obj);
	memfree(pfx);
	memfree(desc);
}
