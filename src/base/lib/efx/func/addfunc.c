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

static void del_func(Func_t **func)
{
	if	(func && func[0])
	{
		message(NULL, MSG_EFMAIN, 203, 1, func[0]->name);
		rd_deref(func[0]);
	}
}


static void set_virfunc(VirFunc_t *vf, Func_t *func)
{
	if	(vf == NULL)
	{
		message(NULL, MSG_EFMAIN, 202, 1, func->name);
		rd_deref(func);
		return;
	}

	if	(func->virtual == 0)
	{
		func->virtual = 1;
		message(NULL, MSG_EFMAIN, 205, 1, func->name);
	}

	del_func(vb_search(&vf->tab, &func, FuncComp, XS_REPLACE));
}


/*	Konverter
*/

static int kfunc_cmp(const void *pa, const void *pb)
{
	Func_t *a, *b;

	a = Val_func(pa);
	b = Val_func(pb);

	if	(a->type == b->type)	return 0;
	else if	(a->weight < b->weight)	return -1;
	else if	(a->weight > b->weight)	return 1;
	else if	(a->type < b->type)	return -1;
	else if	(a->type > b->type)	return 1;
	else				return 0;
}

void CopyKonv (Type_t *type, Type_t *base);

void CopyKonv (Type_t *type, Type_t *base)
{
	Func_t **ftab;
	Func_t *func;
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
		func->arg = ALLOC(1, FuncArg_t);
		func->arg[0].type = type;
		func->arg[0].name = mstrcpy("this");
		func->arg[0].lval = 0;
		func->arg[0].nokonv = 1;
		func->arg[0].cnst = 1;
		func->arg[0].defval = NULL;
		func->eval = ftab[i]->eval;
		func->bound = ftab[i]->bound;
		func->weight = ftab[i]->weight;
		func->virtual = ftab[i]->virtual;
		func->vaarg = 0;
		func->par = ftab[i]->par;
		func->clean = NULL;
	#endif

		del_func(vb_search(&type->konv, &func,
			kfunc_cmp, XS_REPLACE));
	}
}

static void set_kfunc(vecbuf_t *vb, Func_t *func)
{
	del_func(vb_search(vb, &func, kfunc_cmp, XS_REPLACE));
}


/*	Typegebundene Funktionen
*/

static Obj_t *m_func (const Var_t *st, const Obj_t *obj)
{
	ObjFunc_t ofunc;

	ofunc.obj = RefObj(obj);
	ofunc.func = rd_refer(Val_ptr(st->data));
	return NewObj(&Type_ofunc, &ofunc);
}

static void AddTypeFunc(Func_t *func)
{
	Var_t *var;
	Type_t *type;

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

	var = skey_find(&type->vtab->tab, func->name);

	if	(var == NULL)
	{
		if	(func->virtual)
		{
			var = NewVar(&Type_vfunc, func->name, 0);
			Val_vfunc(var->data) = VirFunc(func);
		}
		else
		{
			var = NewVar(&Type_func, func->name, 0);
			Val_func(var->data) = func;
		}

		var->member = m_func;
		AddVar(type->vtab, var, 1);
	}
	else if	(var->type == &Type_vfunc)
	{
		set_virfunc(Val_vfunc(var->data), func);
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
		reg_cpy(1, func->name);
		reg_cpy(2, type->name);
		errmsg(MSG_EFMAIN, 201);
	}
}


void AddFunc(Func_t *func)
{
	if	(func == NULL)	return;

	if	(FuncDebugFlag)
	{
		io_puts("new\t", ioerr);
		ListFunc(ioerr, func);
		io_putc('\n', ioerr);
	}

	if	(func->bound)
	{
		AddTypeFunc(func);
	}
	else if	(func->name == NULL)
	{
		Type_t *type = func->type;

		if	(type->create)
		{
			set_virfunc(type->create, func);
		}
		else	type->create = VirFunc(func);
	}
	else
	{
		Obj_t *obj = GetVar(func->scope, func->name, NULL);

		if	(obj == NULL)
		{
			Var_t *var;

			if	(func->virtual)
			{
				var = NewVar(&Type_vfunc, func->name, 0);
				Val_vfunc(var->data) = VirFunc(func);
			}
			else
			{
				var = NewVar(&Type_func, func->name, 0);
				Val_func(var->data) = func;
			}

			AddVar(func->scope, var, 1);
		}
		else	set_virfunc(Obj2Ptr(obj, &Type_vfunc), func);
	}
}


void *GetStdFunc(VarTab_t *tab, const char *name)
{
	Var_t *var;

	var = skey_find(&tab->tab, name);

	if	(var == NULL)
		return NULL;
	else if	(var->type == &Type_vfunc)
		return rd_refer(Val_vfunc(var->data));
	else if	(var->type == &Type_func)
		return rd_refer(Val_func(var->data));
	else
		return NULL;
}


void *GetTypeFunc(const Type_t *type, const char *name)
{
	Var_t *st;
	const Type_t *x;

	if	(name == NULL)
		return NULL;

	st = NULL;

	for (x = type; st == NULL && x != NULL; x = x->base)
		st = skey_find(&x->vtab->tab, name);

	if	(st == NULL)
		st = skey_find(&Type_obj.vtab->tab, name);

	if	(st == NULL)	return NULL;
	else if	(st->type == &Type_vfunc)
		return rd_refer(Val_vfunc(st->data));
	else if	(st->type == &Type_func)
		return rd_refer(Val_func(st->data));
	else
		return NULL;
}
