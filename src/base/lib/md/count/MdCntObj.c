/*
:*:provide data object for counting
:de:Datenobjekt für Zählung bereitstellen

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
#include <EFEU/Debug.h>

typedef struct {
	MDCOUNT_VAR;
	EfiObj *obj;
	EfiStruct *st;
	EfiFunc *f_add;
} ObjCount;

static ALLOCTAB(buf_count, "ObjCount", 100, sizeof(ObjCount));

static EfiFunc *get_add (EfiType *type)
{
	return GetFunc(type, GetStdFunc(type->vtab, "+="),
		2, type, 1, type, 0); 
}

static void add_std (MdCount *p_cnt, void *data, void *buf)
{
	ObjCount *cnt = (ObjCount *) p_cnt;
	UpdateLval(cnt->obj);
	CopyData(cnt->obj->type, data, cnt->obj->data);
}

static int set_std (MdCount *p_cnt, void *buf, const void *data)
{
	ObjCount *cnt = (ObjCount *) p_cnt;
	UpdateLval(cnt->obj);
	CopyData(cnt->obj->type, buf, cnt->obj->data);
	return 1;
}

static void add_func (MdCount *p_cnt, void *data, void *buf)
{
	ObjCount *cnt = (ObjCount *) p_cnt;
	void *arg[2];
	arg[0] = data;
	arg[1] = buf;
	cnt->f_add->eval(cnt->f_add, NULL, arg);
}

void MdCntObj (MdCountPar *tab, EfiObj *obj, char *pfx, char *desc)
{
	ObjCount *key;
	EfiStruct *st;

	if	(obj->type->dim)
	{
		UnrefObj(obj);
		memfree(pfx);
		memfree(desc);
		return;
	}

	key = new_data(&buf_count);
	key->name = pfx;
	key->type = obj->type->name;
	key->desc = mstrcpy(desc);
	key->set = NULL;
	key->init = NULL;
	key->f_add = get_add(obj->type);

	if	(key->f_add)
	{
		key->set = set_std;
		key->add = add_func;
	}
	else	key->add = add_std;

	key->obj = obj;
	key->st = NULL;
	AddMdCount(tab, key);

	for (st = obj->type->list; st; st = st->next)
	{
		EfiObj *sub;
		char *name;

		if	(st->dim)	continue;
		if	(st->type->dim)	continue;

		sub = Var2Obj(st, obj);
		name = mstrpaste(".", pfx, st->name);
		MdCntObj(tab, sub, name, mstrcpy(st->desc));
	}
}
