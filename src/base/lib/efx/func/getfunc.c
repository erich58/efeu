/*
Funktionen

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
#include <EFEU/conv.h>

typedef struct {
	EfiFunc *func;	/* Konvertierungsfunktion */
	EfiType *type;	/* Konvertierungstype */
	void *data;	/* Datenbuffer */
} ARGKONV;

typedef struct {
	EfiFunc *func;	/* Basisfunktion */
	ARGKONV *konv;	/* Konvertierungsliste */
	size_t dim;	/* Konvertierungsdimension */
	char *buf;	/* Konvertierungsbuffer */
} FUNCDEF;


/*	Funktion für gegebene Argumente aus Funktionstabelle generieren
*/

static void GetFuncEval (EfiFunc *func, void *rval, void **ptr);


EfiFunc *XGetFunc (EfiType *type, EfiVirFunc *vtab,
	EfiFuncArg *arg, size_t narg)
{
	FUNCDEF *def;
	EfiFunc *func;
	EfiArgConv *konv;
	size_t i, j;

	if	(vtab == NULL)	return NULL;

	konv = NULL;
	func = SearchFunc(VirFunc((void *) vtab), arg, narg, &konv);
	rd_deref(vtab);

/*	Keine Konvertierung notwendig
*/
	if	(konv == NULL)
		return rd_refer(func);

/*	Konvertierungsdefinition generieren
*/
	for (i = j = 0; i < narg; i++)
	{
		if	(i >= func->dim - func->vaarg)
			break;

		if	(konv[i].type || konv[i].func)
			j += func->arg[i].type->size;
	}

	def = memalloc(sizeof(FUNCDEF) + i * sizeof(ARGKONV) + j);
	def->func = func;
	def->dim = i;
	def->konv = (ARGKONV *) (def + 1);
	def->buf = (char *) (def->konv + def->dim);

	for (i = j = 0; i < def->dim; i++)
	{
		def->konv[i].func = konv[i].func;
		def->konv[i].type = konv[i].type;

		if	(konv[i].type || konv[i].func)
		{
			def->konv[i].data = def->buf + j;
			j += func->arg[i].type->size;
		}
		else	def->konv[i].data = NULL;
	}

	memfree(konv);

/*	Neue Funktion definieren
*/
	func = NewFunc();

	if	(type == &Type_obj)	func->type = NULL;
	else if	(type == NULL)		func->type = def->func->type;
	else				func->type = type;

	func->name = mstrcpy(def->func->name);
	func->arg = memalloc(narg * sizeof(EfiFuncArg));
	func->eval = GetFuncEval;
	func->dim = narg;
	func->bound = def->func->bound;
	func->weight = def->func->weight;
	func->virfunc = def->func->virfunc;
	func->vaarg = 0;
	func->par = def;
	func->clean = memfree;

	for (i = 0; i < narg; i++)
	{
		func->arg[i].type = arg[i].type;
		func->arg[i].name = mstrcpy(arg[i].name);
		func->arg[i].desc = NULL;
		func->arg[i].lval = arg[i].lval;
		func->arg[i].noconv = 1;
		func->arg[i].cnst = 1;
		func->arg[i].defval = NULL;
	}

	return func;
}


EfiFuncArg *FuncArg (int narg, ...)
{
	va_list list;
	EfiFuncArg *arg;

	va_start(list, narg);
	arg = VaFuncArg(narg, list);
	va_end(list);
	return arg;
}


EfiFuncArg *VaFuncArg (int narg, va_list list)
{
	EfiFuncArg *arg;
	int i;

	if	(narg == 0)	return NULL;

	arg = memalloc(narg * sizeof(EfiFuncArg));

	for (i = 0; i < narg; i++)
	{
		arg[i].type = va_arg(list, EfiType *);
		arg[i].lval = va_arg(list, int);
		arg[i].noconv = 0;
		arg[i].name = NULL;
		arg[i].desc = NULL;
		arg[i].defval = NULL;
	}

	return arg;
}


EfiFunc *GetFunc (EfiType *type, EfiVirFunc *tab, int narg, ...)
{
	EfiFunc *func;
	EfiFuncArg *arg;
	va_list list;

	va_start(list, narg);
	arg = VaFuncArg(narg, list);
	va_end(list);
	func = XGetFunc(type, tab, arg, narg);
	memfree(arg);
	return func;
}


EfiFunc *VaGetFunc (EfiType *type, EfiVirFunc *tab, int narg, va_list list)
{
	EfiFunc *func;
	EfiFuncArg *arg;

	arg = VaFuncArg(narg, list);
	func = XGetFunc(type, tab, arg, narg);
	memfree(arg);
	return func;
}

/*	Auswertungsfunktion
*/

static void GetFuncEval (EfiFunc *func, void *rval, void **ptr)
{
	FUNCDEF *def;
	ARGKONV *konv;
	EfiFuncArg *arg;
	EfiFuncArg *farg;
	EfiObjList *list, *vlist, **p;
	void **arglist;
	EfiObj *o;
	int i, n;

	def = func->par;
	arglist = memalloc(def->func->dim * sizeof(void *));

/*	Aufrufargumente konvertieren
*/
	arg = func->arg;
	farg = def->func->arg;
	konv = def->konv;

	for (i = 0; i < def->dim; i++)
	{
		if	(arg[i].type == farg[i].type)
		{
			arglist[i] = ptr[i];
		}
		else if	(arg[i].type == NULL)
		{
			EfiObj *o;

			o = EvalObj(RefObj(ptr[i]), farg[i].type);

			if	(o == NULL)
				o = NewObj(farg[i].type, NULL);

			arglist[i] = o->data;
			konv[i].data = o;
		}
		else if	(farg[i].type == NULL)
		{
			if	(farg[i].lval)
			{
				arglist[i] = LvalObj(&Lval_ptr, arg[i].type,
					ptr[i]);
			}
			else	arglist[i] = ConstObj(arg[i].type, ptr[i]);
		}
		else if	(konv[i].func)
		{
			register EfiFunc *func = konv[i].func;

			arglist[i] = konv[i].data;
			memset(arglist[i], 0, farg[i].type->size);
			func->eval(func, arglist[i], ptr + i);
		}
		else if	(konv[i].type)
		{
			arglist[i] = konv[i].data;
			memset(arglist[i], 0, farg[i].type->size);
			CopyData(konv[i].type, arglist[i], ptr[i]);
		}
		else	arglist[i] = ptr[i];
	}

/*	Erweitern mit Vorgabewerten
*/
	list = NULL;
	p = &list;
	n = def->func->dim - def->func->vaarg;

	for (i = def->dim; i < n; i++)
	{
		o = EvalObj(RefObj(farg[i].defval), farg[i].type);

		if	(o == NULL && farg[i].type)
			o = NewObj(farg[i].type, NULL);
			
		arglist[i] = (farg[i].type ? o->data : o);
		*p = NewObjList(o);
		p = &(*p)->next;
	}

/*	Variable Argumentliste
*/
	vlist = NULL;
	p = &vlist;

	for (i = n; i < func->dim; i++)
	{
		if	(arg[i].type == NULL)
		{
			o = RefObj(ptr[i]);
		}
		else if	(arg[i].lval)
		{
			o = LvalObj(&Lval_ptr, arg[i].type, ptr[i]);
		}
		else	o = ConstObj(arg[i].type, ptr[i]);

		*p = NewObjList(o);
		p = &(*p)->next;
	}

	if	(def->func->vaarg)
		arglist[n] = &vlist;

/*	Auswerten und Aufräumen
*/
	if	(def->func->lretval)
	{
		def->func->eval(def->func, NULL, arglist);
	}
	else if	(func->type != def->func->type)
	{
		Obj2Data(MakeRetVal(def->func, NULL, arglist),
			func->type, rval);
	}
	else	def->func->eval(def->func, rval, arglist);

	for (i = 0; i < def->dim; i++)
	{
		if	(arg[i].type == farg[i].type)
		{
			;
		}
		else if	(arg[i].type == NULL)
		{
			UnrefObj(konv[i].data);
		}
		else if	(farg[i].type == NULL)
		{
			UnrefObj(arglist[i]);
		}
		else if	(konv[i].func || konv[i].type)
		{
			CleanData(farg[i].type, arglist[i], 1);
		}
	}

	memfree(arglist);
	DelObjList(list);
	DelObjList(vlist);
}
