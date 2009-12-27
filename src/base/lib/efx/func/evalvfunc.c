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

static void clean_arg (FuncArg_t *arg, size_t narg);
static int set_arg (Func_t *func, FuncArg_t *arg, size_t narg, Obj_t *obj);


Obj_t *Expr_virfunc (void *par, const ObjList_t *list)
{
	return EvalVirFunc(par, list);
}

Obj_t *Expr_void(void *par, const ObjList_t *list)
{
	while (list != NULL)
	{
		UnrefEval(RefObj(list->obj));
		list = list->next;
	}

	return NULL;
}


Obj_t *EvalVirFunc(VirFunc_t *vtab, const ObjList_t *list)
{
	ArgKonv_t *fkonv;
	Func_t *func, **ftab;
	FuncArg_t *arg;
	size_t argdim;
	size_t maxdim;
	size_t fargdim;
	Obj_t *obj;
	int i;

	if	(vtab == NULL || vtab->tab.used == 0)
		return NULL;

	if	(vtab->reftype != &VirFuncRefType)
	{
		liberror(MSG_EFMAIN, 65);
		return NULL;
	}

/*	Argumentzahl und maximale Dimension bestimmen
*/
	argdim = ObjListLen(list);
	maxdim = argdim;
	func = NULL;
	ftab = vtab->tab.data;

	for (i = 0; i < vtab->tab.used; i++)
	{
		func = ftab[i];

		if (maxdim < func->dim)	maxdim = func->dim;
	}

/*	Funktionsargumente auswerten
*/
	arg = ALLOC(maxdim, FuncArg_t);
	memset(arg, 0, maxdim * sizeof(FuncArg_t));

	for (i = 0; list != NULL; i++)
	{
		if	(!set_arg(func, arg, i, EvalObj(RefObj(list->obj), NULL)))
			return NULL;

		list = list->next;
	}

/*	Funktion mit minimaler Distanz suchen
*/
	func = SearchFunc(vtab, arg, argdim, &fkonv);

/*	Keine Funktion für passende Argumente: Fehlermeldung generieren
*/
	if	(func == NULL || func->eval == NULL)
	{
		strbuf_t *sb;
		char *delim;

		sb = new_strbuf(0);
		sb_puts(ftab[0]->name, sb);
		sb_putc('(', sb);
		delim = NULL;

		for (i = 0; i < argdim; i++)
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
		reg_set(1, sb2str(sb));
		errmsg(MSG_EFMAIN, func ? 91 : 92);
		clean_arg(arg, argdim);
		return NULL;
	}

/*	Argumente konvertieren
*/
	if	(fkonv != NULL)
	{
		for (i = 0; i < argdim; i++)
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

/*	Bei variablen Argumenten Liste generieren
*/
	if	(func->vaarg && (argdim != func->dim ||
		arg[fargdim].defval->type != &Type_list))
	{
		ObjList_t *vlist, *x;

		vlist = NULL;

		while (argdim > fargdim)
		{
			argdim--;
			x = NewObjList(arg[argdim].defval);
			x->next = vlist;
			vlist = x;
		}

		set_arg(func, arg, argdim, Obj_list(vlist));
		argdim++;
	}

/*	Vorgabewerte einsetzen
*/
	while (argdim < fargdim)
	{
		obj = EvalObj(RefObj(func->arg[argdim].defval),
			func->arg[argdim].type);

		if	(set_arg(func, arg, argdim, obj))
		{
			argdim++;
		}
		else	return NULL;
	}

	FuncDebug(func, "eval");

/*	Funktion ausführen
*/
	if	(argdim)
	{
		void **arglist = ALLOC(argdim, void *);

		for (i = 0; i < argdim; i++)
		{
			arglist[i] = func->arg[i].type ?
				arg[i].defval->data : arg[i].defval;
		}

		obj = MakeRetVal(func, arg[i].defval, arglist);
		memfree(arglist);

		for (i = 0; i < argdim; i++)
		{
			if	(func->arg[i].lval)
				SyncLval(arg[i].defval);
		}
	}
	else	obj = MakeRetVal(func, NULL, NULL);

	clean_arg(arg, argdim);
	return obj;
}


/*	Argument initialisieren
*/

static int set_arg (Func_t *func, FuncArg_t *arg, size_t narg, Obj_t *obj)
{
	if	(obj == NULL)
	{
		reg_str(1, func->name);
		reg_fmt(2, "%d", narg + 1 - func->bound);
		errmsg(MSG_EFMAIN, 94);
		clean_arg(arg, narg);
		return 0;
	}

	arg[narg].defval = obj;
	arg[narg].type = obj->type;
	arg[narg].lval = obj->lval ? 1 : 0;
	arg[narg].nokonv = 1;
	return 1;
}


/*	Argumentvektor Aufräumen
*/

static void clean_arg (FuncArg_t *arg, size_t narg)
{
	int i;

	for (i = 0; i < narg; i++)
		UnrefObj(arg[i].defval);

	memfree(arg);
}
