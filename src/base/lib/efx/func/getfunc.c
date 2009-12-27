/*	Funktionen
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/konvobj.h>

typedef struct {
	Func_t *func;	/* Konvertierungsfunktion */
	Type_t *type;	/* Konvertierungstype */
	void *data;	/* Datenbuffer */
} ARGKONV;

typedef struct {
	Func_t *func;	/* Basisfunktion */
	ARGKONV *konv;	/* Konvertierungsliste */
	size_t dim;	/* Konvertierungsdimension */
	char *buf;	/* Konvertierungsbuffer */
} FUNCDEF;


/*	Funktion f�r gegebene Argumente aus Funktionstabelle generieren
*/

static void GetFuncEval (Func_t *func, void *rval, void **ptr);


Func_t *XGetFunc(Type_t *type, VirFunc_t *vtab, FuncArg_t *arg, size_t narg)
{
	FUNCDEF *def;
	Func_t *func;
	ArgKonv_t *konv;
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

	FREE(konv);

/*	Neue Funktion definieren
*/
	func = NewFunc();

	if	(type == &Type_obj)	func->type = NULL;
	else if	(type == NULL)		func->type = def->func->type;
	else				func->type = type;

	func->name = mstrcpy(def->func->name);
	func->arg = ALLOC(narg, FuncArg_t);
	func->eval = GetFuncEval;
	func->dim = narg;
	func->bound = def->func->bound;
	func->weight = def->func->weight;
	func->virtual = def->func->virtual;
	func->vaarg = 0;
	func->par = def;
	func->clean = memfree;

	for (i = 0; i < narg; i++)
	{
		func->arg[i].type = arg[i].type;
		func->arg[i].name = mstrcpy(arg[i].name);
		func->arg[i].lval = arg[i].lval;
		func->arg[i].nokonv = 1;
		func->arg[i].cnst = 1;
		func->arg[i].defval = NULL;
	}

	return func;
}


FuncArg_t *FuncArg(int narg, ...)
{
	va_list list;
	FuncArg_t *arg;

	va_start(list, narg);
	arg = VaFuncArg(narg, list);
	va_end(list);
	return arg;
}


FuncArg_t *VaFuncArg(int narg, va_list list)
{
	FuncArg_t *arg;
	int i;

	if	(narg == 0)	return NULL;

	arg = ALLOC(narg, FuncArg_t);

	for (i = 0; i < narg; i++)
	{
		arg[i].type = va_arg(list, Type_t *);
		arg[i].lval = va_arg(list, int);
		arg[i].nokonv = 0;
		arg[i].name = NULL;
		arg[i].defval = NULL;
	}

	return arg;
}


Func_t *GetFunc (Type_t *type, VirFunc_t *tab, int narg, ...)
{
	Func_t *func;
	FuncArg_t *arg;
	va_list list;

	va_start(list, narg);
	arg = VaFuncArg(narg, list);
	va_end(list);
	func = XGetFunc(type, tab, arg, narg);
	FREE(arg);
	return func;
}


Func_t *VaGetFunc (Type_t *type, VirFunc_t *tab, int narg, va_list list)
{
	Func_t *func;
	FuncArg_t *arg;

	arg = VaFuncArg(narg, list);
	func = XGetFunc(type, tab, arg, narg);
	FREE(arg);
	return func;
}


static void va_func_ptr_eval(Func_t *func, void *rval, va_list list)
{
	void **args;
	int n;

	if	(func->dim)
	{
		args = memalloc(func->dim * sizeof(void *));

		for (n = 0; n < func->dim; n++)
			args[n] = va_arg(list, void *);

	}
	else	args = NULL;

	func->eval(func, rval, args);
	memfree(args);
}

static Obj_t *va_func_obj_eval(Func_t *func, va_list list)
{
	Obj_t *obj = NULL;

	if	(func->type == NULL)
	{
		obj = NULL;
		va_func_ptr_eval(func, &obj, list);
	}
	else if	(func->lretval)
	{
		obj = LvalObj(func->type, func, NULL);
		va_func_ptr_eval(func, &obj->data, list);
	}
	else
	{
		obj = NewObj(func->type, NULL);
		va_func_ptr_eval(func, obj->data, list);
	}

	return obj;
}

void CallFunc (Type_t *type, void *ptr, Func_t *func, ...)
{
	va_list list;

	if	(func == NULL)	return;

	va_start(list, func);

	if	(type != func->type)
	{
		Obj2Data(va_func_obj_eval(func, list), type, ptr);
	}
	else	va_func_ptr_eval(func, ptr, list);

	va_end(list);
}


/* VARARGS 2 */

void CallVoidFunc (Func_t *func, ...)
{
	if	(func)
	{
		va_list list;

		va_start(list, func);
		UnrefObj(va_func_obj_eval(func, list));
		va_end(list);
	}
}


/* VARARGS 2 */

Obj_t *CallFuncObj (Func_t *func, ...)
{
	if	(func)
	{
		va_list list;
		Obj_t *obj;

		va_start(list, func);
		obj = va_func_obj_eval(func, list);
		va_end(list);
		return obj;
	}
	else	return NULL;
}


/*	Auswertungsfunktion
*/

static void GetFuncEval(Func_t *func, void *rval, void **ptr)
{
	FUNCDEF *def;
	ARGKONV *konv;
	FuncArg_t *arg;
	FuncArg_t *farg;
	ObjList_t *list, *vlist, **p;
	void **arglist;
	Obj_t *o;
	int i, n;

	def = func->par;
	arglist = ALLOC(def->func->dim, void *);

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
			Obj_t *o;

			o = EvalObj(ptr[i], farg[i].type);

			if	(o == NULL)
				o = NewObj(farg[i].type, NULL);

			arglist[i] = o->data;
			konv[i].data = o;
		}
		else if	(farg[i].type == NULL)
		{
			if	(farg[i].lval)
			{
				arglist[i] = LvalObj(arg[i].type, NULL, ptr[i]);
			}
			else	arglist[i] = ConstObj(arg[i].type, ptr[i]);
		}
		else if	(konv[i].func)
		{
			register Func_t *func = konv[i].func;

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
			o = LvalObj(arg[i].type, NULL, ptr[i]);
		}
		else	o = ConstObj(arg[i].type, ptr[i]);

		*p = NewObjList(o);
		p = &(*p)->next;
	}

	if	(def->func->vaarg)
		arglist[n] = &vlist;

/*	Auswerten und Aufr�umen
*/
	KonvRetVal(func->type, rval, def->func, arglist);

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
			CleanData(farg[i].type, arglist[i]);
		}
	}

	FREE(arglist);
	DelObjList(list);
	DelObjList(vlist);
}
