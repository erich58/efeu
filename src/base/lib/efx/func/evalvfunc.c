/*
Funktionen bestimmen und auswerten

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
#include <EFEU/konvobj.h>
#include <EFEU/printobj.h>

/*	Argumentvektor aufräumen
*/

static void clean_arg (EfiFuncArg *arg, size_t narg)
{
	int i;

	for (i = 0; i < narg; i++)
		UnrefObj(arg[i].defval);

	memfree(arg);
}


/*	Argument initialisieren
*/

static int set_arg (EfiFunc *func, EfiFuncArg *arg, size_t narg, EfiObj *obj)
{
	if	(obj == NULL)
	{
		dbg_note(NULL, "[efmain:94]", "sd", func->name,
			narg + 1 - func->bound);
		clean_arg(arg, narg);
		return 0;
	}

	arg[narg].defval = obj;
	arg[narg].type = obj->type;
	arg[narg].lval = obj->lval ? 1 : 0;
	arg[narg].nokonv = 1;
	return 1;
}

/*	Test auf virtuelle Funktion
*/

static int not_a_vfunc (EfiVirFunc *vtab)
{
	if	(vtab == NULL || vtab->tab.used == 0)
		return 1;

	if	(IsVirFunc(vtab))	return 0;

	dbg_error(NULL, "[efmain:65]", NULL);
	return 1;
}

/*	Maximale Dimension bestimmen
*/

static size_t get_maxdim (EfiVirFunc *vtab, size_t maxdim)
{
	EfiFunc **func;
	size_t n;

	for (func = vtab->tab.data, n = vtab->tab.used; n-- > 0; func++)
		if (maxdim < (*func)->dim) maxdim = (*func)->dim;

	return maxdim;
}


/*	Argumentliste aus Objektliste bestimmen
*/

static EfiFuncArg *farg_list (EfiVirFunc *vtab,
	const EfiObjList *list, size_t dim)
{
	EfiFuncArg *arg;
	EfiFunc *func;
	int i;

	arg = memalloc(dim * sizeof(EfiFuncArg));
	func = ((EfiFunc **) vtab->tab.data)[0];

	for (i = 0; list != NULL; i++)
	{
		if	(!set_arg(func, arg, i,
				EvalObj(RefObj(list->obj), NULL)))
			return NULL;

		list = list->next;
	}

	return arg;
}

/*	Funktion bestimmen
*/

static EfiFunc *get_func (EfiVirFunc *vtab, EfiFuncArg *arg,
	size_t narg, EfiArgKonv **ptr)
{
	EfiFunc *func;
	StrBuf *sb;
	char *delim;
	int i;

	func = SearchFunc(vtab, arg, narg, ptr);

	if	(func && func->eval)	return func;

	sb = sb_create(0);
	sb_puts(((EfiFunc **) vtab->tab.data)[0]->name, sb);
	sb_putc('(', sb);
	delim = NULL;

	for (i = 0; i < narg; i++)
	{
		sb_puts(delim, sb);

		if	(arg[i].type == &Type_undef)
		{
			sb_puts(Type_undef.name, sb);
			sb_putc(' ', sb);
			sb_puts(Val_str(arg[i].defval->data), sb);
		}
		else	sb_puts(arg[i].type->name, sb);

		delim = ", ";
	}

	sb_putc(')', sb);
	dbg_note(NULL, func ? "[efmain:91]" : "[efmain:92]", "m", sb2str(sb));
	clean_arg(arg, narg);
	return NULL;
}

/*	Funktion auswerten
*/

static EfiObj *eval_func (EfiFunc *func, EfiArgKonv *fkonv,
	EfiFuncArg *arg, size_t narg)
{
	size_t fargdim;
	EfiObj *obj;
	int i;

	if	(fkonv)
	{
		for (i = 0; i < narg; i++)
		{
			if	(fkonv[i].func || fkonv[i].type)
			{
				obj = NewObj(func->arg[i].type, NULL);
				ArgKonv(fkonv + i, obj->data,
					arg[i].defval->data);
				UnrefObj(arg[i].defval);
				arg[i].defval = obj;
				arg[i].type = obj->type;
				arg[i].lval = obj->lval ? 1 : 0;
				arg[i].nokonv = 1;
			}
		}

		memfree(fkonv);
	}

	fargdim = func->dim - func->vaarg;

/*	Vorgabewerte einsetzen
*/
	while (narg < fargdim)
	{
		obj = EvalObj(RefObj(func->arg[narg].defval),
			func->arg[narg].type);

		if	(!set_arg(func, arg, narg, obj))
			return NULL;

		narg++;
	}

/*	Bei variablen Argumenten Liste generieren
*/
	if	(func->vaarg && (narg != func->dim ||
		arg[fargdim].defval->type != &Type_list))
	{
		EfiObjList *vlist, *x;

		vlist = NULL;

		while (narg > fargdim)
		{
			narg--;
			x = NewObjList(arg[narg].defval);
			x->next = vlist;
			vlist = x;
		}

		set_arg(func, arg, narg, Obj_list(vlist));
		narg++;
	}

	FuncDebug(func, "eval");

/*	Funktion ausführen
*/
	if	(narg)
	{
		void **arglist = memalloc(narg * sizeof(void *));

		for (i = 0; i < narg; i++)
			arglist[i] = func->arg[i].type ?
				arg[i].defval->data : arg[i].defval;

		obj = MakeRetVal(func, arg[0].defval, arglist);
		memfree(arglist);

		for (i = 0; i < narg; i++)
			SyncLval(arg[i].defval);
	}
	else	obj = MakeRetVal(func, NULL, NULL);

	clean_arg(arg, narg);
	return obj;
}


EfiObj *EvalVirFunc (EfiVirFunc *vtab, const EfiObjList *list)
{
	EfiArgKonv *fkonv;
	EfiFuncArg *arg;
	EfiFunc *func;
	size_t argdim;
	size_t maxdim;

	if	(not_a_vfunc(vtab))
		return NULL;

	argdim = ObjListLen(list);
	maxdim = get_maxdim(vtab, argdim);
	arg = NULL;

	if	(maxdim && !(arg = farg_list(vtab, list, maxdim)))
		return NULL;

	if	(!(func = get_func(vtab, arg, argdim, &fkonv)))
		return NULL;

	return eval_func(func, fkonv, arg, argdim);
}


int ShowData (IO *io, const EfiType *type, const void *data)
{
	EfiVirFunc *vtab;
	EfiFunc *func;
	EfiArgKonv *fkonv;
	EfiFuncArg *arg;
	size_t maxdim;

	vtab = GetGlobalFunc("fprint");

	if	(not_a_vfunc(vtab))
		return ShowAny(io, type, data);

	maxdim = get_maxdim(vtab, 2);
	arg = memalloc(maxdim * sizeof(EfiFuncArg));
	func = ((EfiFunc **) vtab->tab.data)[0];
	set_arg(func, arg, 0, LvalObj(&Lval_ptr, &Type_io, &io));
	set_arg(func, arg, 1, LvalObj(&Lval_ptr, (EfiType *) type, data));

	if	((func = get_func(vtab, arg, 2, &fkonv)))
		return Obj2int(eval_func(func, fkonv, arg, 2));

	return ShowAny(io, type, data);
}

EfiObj *Expr_virfunc (void *par, const EfiObjList *list)
{
	return EvalVirFunc(par, list);
}

EfiObj *Expr_void (void *par, const EfiObjList *list)
{
	while (list != NULL)
	{
		UnrefEval(RefObj(list->obj));
		list = list->next;
	}

	return NULL;
}
