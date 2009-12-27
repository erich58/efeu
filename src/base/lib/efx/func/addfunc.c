/*
Funktion in Variablenliste aufnehmen

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

static void del_func (EfiFunc **func)
{
	if	(func && func[0])
	{
		log_note(NULL, "[efmain:203]", "s",
			func[0]->name ? func[0]->name :
			(func[0]->type ? func[0]->type->name : NULL));
		rd_deref(func[0]);
	}
}


static void set_virfunc (EfiVirFunc *vf, EfiFunc *func)
{
	if	(vf == NULL)
	{
		log_note(NULL, "[efmain:202]", "s", func->name);
		rd_deref(func);
		return;
	}

	if	(func->virfunc == 0)
	{
		func->virfunc = 1;
		log_note(NULL, "[efmain:205]", "s", func->name);
	}

	del_func(vb_search(&vf->tab, &func, FuncComp, VB_REPLACE));
}


/*	Konverter
*/

static int kfunc_cmp(const void *pa, const void *pb)
{
	EfiFunc *a, *b;

	a = Val_func(pa);
	b = Val_func(pb);

	if	(a->type == b->type)	return 0;
	else if	(a->weight < b->weight)	return -1;
	else if	(a->weight > b->weight)	return 1;
	else if	(a->type < b->type)	return -1;
	else if	(a->type > b->type)	return 1;
	else				return 0;
}

void CopyKonv (EfiType *type, EfiType *base);

void CopyKonv (EfiType *type, EfiType *base)
{
	EfiFunc **ftab;
	EfiFunc *func;
	int i;

	ftab = base->konv.data;

	for (i = 0; i < base->konv.used; i++)
	{
		func = rd_refer(ftab[i]);
	#if	0
		if	(ftab[i]->clean)
			continue;

		func = NewFunc();
		func->type = ftab[i]->type;
		func->name = NULL;
		func->dim = 1;
		func->arg = memalloc(sizeof(EfiFuncArg));
		func->arg[0].type = type;
		func->arg[0].name = mstrcpy("this");
		func->arg[0].lval = 0;
		func->arg[0].nokonv = 1;
		func->arg[0].cnst = 1;
		func->arg[0].defval = NULL;
		func->eval = ftab[i]->eval;
		func->bound = ftab[i]->bound;
		func->weight = ftab[i]->weight;
		func->virfunc = ftab[i]->virfunc;
		func->vaarg = 0;
		func->par = ftab[i]->par;
		func->clean = NULL;
	#endif

		del_func(vb_search(&type->konv, &func,
			kfunc_cmp, VB_REPLACE));
	}
}

static void set_kfunc(VecBuf *vb, EfiFunc *func)
{
	del_func(vb_search(vb, &func, kfunc_cmp, VB_REPLACE));
}


/*	Typegebundene Funktionen
*/

static EfiObj *get_func (const EfiObj *obj, void *data)
{
	EfiObjFunc ofunc;

	ofunc.obj = RefObj(obj);
	ofunc.func = rd_refer(data);
	return NewObj(&Type_ofunc, &ofunc);
}

static EfiObj *get_vfunc (const EfiObj *obj, void *data)
{
	EfiObjFunc ofunc;

	ofunc.obj = RefObj(obj);
	ofunc.func = rd_refer(data);
	return NewObj(&Type_ofunc, &ofunc);
}

static void func_clean (VarTabEntry *entry)
{
	rd_deref(entry->data);
}

static void AddTypeFunc (EfiFunc *func)
{
	EfiType *type;
	VarTabEntry *var;

	type = func->arg[0].type;

	if	(type == NULL)	type = &Type_obj;

	if	(func->name == NULL)
	{
		if	(func->type == &Type_void)
		{
			del_func(&type->fclean);
			type->fclean = func;
		}
		else if	(func->type == type)
		{
			del_func(&type->fcopy);
			type->fcopy = func;
		}
		else	set_kfunc(&type->konv, func);

		return;
	}

	var = VarTab_get(type->vtab, func->name);

	if	(var == NULL)
	{
		VarTabEntry entry;
		entry.name = func->name;
		entry.desc = NULL;
		entry.type = &Type_ofunc;
		entry.obj = NULL;
		entry.entry_clean = func_clean;

		if	(func->virfunc)
		{
			entry.get = get_vfunc;
			entry.data = VirFunc(func);
		}
		else
		{
			entry.get = get_func;
			entry.data = func;
		}

		VarTab_add(type->vtab, &entry);
	}
	else if	(var->get == get_vfunc)
	{
		set_virfunc(var->data, func);
	}
	/*
	else if	(var->type == &Type_func)
	{
		del_func(&var->par);
		st->par = func;
	}
	*/
	else
	{
		log_note(NULL, "[efmain:201]", "ss", func->name, type->name);
		rd_deref(func);
	}
}


void AddFunc (EfiFunc *func)
{
	if	(func == NULL)	return;

	FuncDebug(func, "new");

	if	(func->bound)
	{
		AddTypeFunc(func);
	}
	else if	(func->name == NULL)
	{
		EfiType *type = func->type;

		if	(!type)
		{
			log_note(NULL, "[efmain:206]", NULL);
			rd_deref(func);
		}
		else if	(type->create)
		{
			set_virfunc(type->create, func);
		}
		else	type->create = VirFunc(func);
	}
	else
	{
		VarTabEntry *var = VarTab_get(func->scope, func->name);

		if	(var == NULL)
		{
			EfiObj *obj;

			if	(func->virfunc)
			{
				obj = NewPtrObj(&Type_vfunc, VirFunc(func));
			}
			else	obj = NewPtrObj(&Type_func, func);

			VarTab_xadd(func->scope, mstrcpy(func->name),
				NULL, obj);
		}
		else if	(var->obj && var->obj->type == &Type_vfunc)
		{
			set_virfunc(Val_ptr(var->obj->data), func);
		}
		else
		{
			log_note(NULL, "[efmain:202]", "s", func->name);
			rd_deref(func);
		}
	}
}


static void *obj2func (EfiObj *obj)
{
	void *func = NULL;

	if	(obj)
	{
		if	(obj->type == &Type_vfunc)
		{
			func = rd_refer(Val_ptr(obj->data));
		}
		else if	(obj->type == &Type_func)
		{
			func = rd_refer(Val_ptr(obj->data));
		}
		else if	(obj->type == &Type_ofunc)
		{
			func = rd_refer(((EfiObjFunc *) obj->data)->func);
		}

		UnrefObj(obj);
	}

	return func;
}

void *GetStdFunc (EfiVarTab *tab, const char *name)
{
	return obj2func(GetVar(tab, name, NULL));
}


void *GetTypeFunc (const EfiType *type, const char *name)
{
	EfiObj *obj;
	const EfiType *x;

	if	(name == NULL)
		return NULL;

	obj = NULL;

	for (x = type; obj == NULL && x != NULL; x = x->base)
		obj = GetVar(x->vtab, name, NULL);

	if	(obj == NULL)
		obj = GetVar(Type_obj.vtab, name, NULL);

	return obj2func(obj);
}
