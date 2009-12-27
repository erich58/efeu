/*
Datentype konstruieren

$Copyright (C) 2006 Erich Frühstück
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
#include <EFEU/printobj.h>
#include <EFEU/parsedef.h>
#include <EFEU/Op.h>
#include <ctype.h>

#define	SHOW_EVAL	0

#define	ERR_TYP	"[edb:typ]$!: unknown datatype $1.\n"
#define ERR_USE	"[edb:use]$!: name $1 arready used.\n"
#define ERR_VAR	"[edb:var]$!: unknown variable $1.\n"

/*	Selektionseinträge
*/

typedef struct ConstructEntry ConstructEntry;

struct ConstructEntry {
	ConstructEntry *next;
	EfiVar *var;
	EfiFunc *func;
	EfiObj *obj;
	EfiVar *base;
	int idx;
};

static ALLOCTAB(entry_tab, 0, sizeof(ConstructEntry));

static ConstructEntry *ConstructEntry_alloc (EfiVar *var)
{
	ConstructEntry *entry = new_data(&entry_tab);
	entry->next = NULL;
	entry->var = rd_refer(var);
	entry->func = NULL;
	entry->obj = NULL;
	entry->base = NULL;
	entry->idx = 0;
	return entry;
}

static void ConstructEntry_clean (ConstructEntry *entry)
{
	if	(entry)
	{
		ConstructEntry_clean(entry->next);
		rd_deref(entry->var);
		rd_deref(entry->func);
		UnrefObj(entry->obj);
		rd_deref(entry->base);
		del_data(&entry_tab, entry);
	}
}
	
/*	Selektionsparameter
*/

typedef struct {
	size_t dim;
	VarTabEntry **var;
	EfiVarTab *tab;
	EfiObj *obj;
	EfiVar *vlist;
	ConstructEntry *root;
	ConstructEntry **ptr;
} ConstructPar;

static void func_noarg (EfiFunc *func, void *rval, void **arg);
static void func_list (EfiFunc *func, void *rval, void **arg);

static EfiObj *farg_obj (EfiFuncArg *arg)
{
	return LvalObj(&Lval_ptr, arg->type, arg->defval ?
		arg->defval->data : arg->type->defval);
}

static void *Construct_alloc (EfiFunc *func)
{
	ConstructPar *par;
	int i;
	
	par = memalloc(sizeof *par + func->dim * sizeof par->var[0]);
	par->dim = func->dim;
	par->var = NULL;
	par->tab = NULL;
	par->obj = NULL;

	if	(par->dim)
	{
		func->eval = func_list;

		if	(func->arg[0].name || par->dim > 1)
		{
			par->var = (void *) (par + 1);
			par->tab = VarTab(NULL, par->dim);

			for (i = 0; i < par->dim; i++)
				VarTab_xadd(par->tab, func->arg[i].name,
					NULL, farg_obj(func->arg + i));

			for (i = 0; i < par->dim; i++)
				par->var[i] = VarTab_get(par->tab,
					func->arg[i].name);
		}
		else
		{
			par->obj = farg_obj(func->arg);
			par->tab = RefVarTab(par->obj->type->vtab);
		}
	}
	
	par->vlist = NULL;
	par->root = NULL;
	par->ptr = &par->root;
	func->par = par;
	return par;
}

static void Construct_clean (void *opaque_par)
{
	ConstructPar *par = opaque_par;
	DelVarList(par->vlist);
	DelVarTab(par->tab);
	UnrefObj(par->obj);
	ConstructEntry_clean(par->root);
	memfree(par);
}

static ConstructEntry *Construct_next (ConstructPar *par, EfiVar *var)
{
	if	(var)
	{
		ConstructEntry *entry = ConstructEntry_alloc(var);
		*par->ptr = entry;
		par->ptr = &(*par->ptr)->next;
		return entry;
	}
	else	return NULL;
}

static void func_noarg (EfiFunc *func, void *rval, void **arg)
{
	ConstructPar *par;
	ConstructEntry *entry;

	par = func->par;

	for (entry = par->root; entry; entry = entry->next)
	{
		void *ptr = (char *) rval + entry->var->offset;

		if	(entry->func)
		{
			entry->func->eval(entry->func, ptr, arg);
		}
		else if	(entry->obj)
		{
			Obj2Data(RefObj(entry->obj), entry->var->type, ptr);
		}
	}
}

static void func_list (EfiFunc *func, void *rval, void **arg)
{
	ConstructPar *par;
	ConstructEntry *entry;
	int i;

	par = func->par;

	if	(par->obj)
	{
		par->obj->data = arg[0];
	}
	else
	{
		for (i = 0; i < par->dim; i++)
			par->var[i]->obj->data = arg[i];
	}

	PushVarTab(RefVarTab(par->tab), RefObj(par->obj));

	for (entry = par->root; entry; entry = entry->next)
	{
		void *ptr = (char *) rval + entry->var->offset;

		if	(entry->func)
		{
			entry->func->eval(entry->func, ptr, arg);
		}
		else if	(entry->obj)
		{
			Obj2Data(RefObj(entry->obj), entry->var->type, ptr);
		}
		else if	(entry->base)
		{
			int n = entry->base->dim ? entry->base->dim : 1;
			void *src = (char *) arg[entry->idx] +
				entry->base->offset;
			AssignVecData(entry->var->type, n, ptr, src);
		}
	}

	PopVarTab();
}

#if	SHOW_EVAL
static void show_eval (ConstructPar *par)
{
	ConstructEntry *entry;

	for (entry = par->root; entry; entry = entry->next)
	{
		io_printf(ioerr, "%s: ", entry->var->name);

		if	(entry->func)
		{
			io_printf(ioerr, "func\n");
		}
		else if	(entry->obj)
		{
			io_printf(ioerr, "expr\n");
		}
		else if	(entry->base)
		{
			io_printf(ioerr, "copy(%d)\n", entry->idx);
		}
		else	io_printf(ioerr, "defval\n");
	}
}
#endif

/*	Funktionsverwaltung
*/

static EfiFunc *cf_create (void)
{
	EfiFunc *func;

	func = NewFunc();
	func->type = NULL;
	func->lretval = 0;
	func->scope = NULL;
	func->name = NULL;
	func->bound = 0;
	func->virfunc = 1;
	func->weight = KONV_STANDARD;
	func->eval = func_noarg;
	func->par = NULL;
	func->clean = Construct_clean;
	func->dim = 0;
	func->arg = NULL;
	return func;
}

/*	Parameter zusammenstellen
*/

static void cf_set (EfiFunc *func, const char *name, IO *io, int delim);

static ConstructEntry *add_var (ConstructPar *par, EfiVar *var, int flag)
{
	EfiVar **p;

	if	(!var)	return NULL;

	for (p = &par->vlist; *p; p = &(*p)->next)
	{
		if	(mstrcmp(var->name, (*p)->name) == 0)
		{
			if	(flag)
				dbg_error("edb", ERR_USE, "s", var->name);

			DelVar(var);
			return NULL;
		}
	}

	*p = var;
	return Construct_next(par, var);
}

static void add_expr (ConstructPar *par, EfiType *type, char *name, EfiObj *obj)
{
	EfiObj *cobj = EvalObj(RefObj(obj), type);

	if	(cobj)
	{
		EfiVar *var;
		ConstructEntry *entry;

		var = NewVar(cobj->type, NULL, 0);
		var->name = name;
		var->member = StructMember;
		var->offset = 0;
		Obj2Data(cobj, var->type, var->data);
		entry = add_var(par, var, 1);

		if	(entry)
			entry->obj = obj;
	}
	else	memfree(name);
}

static void list_var (IO *out, const char *pfx, EfiObj *obj)
{
	PrintType(out, obj->type, 2);

	if	(pfx)
	{
		io_putc(' ', out);
	}
	else	pfx = "";

	ShowVarTab(out, pfx, obj->type->vtab);
	io_putc('\n', out);
}

static void add_member (IO *io, ConstructPar *par,
	EfiType *type, char *name, int idx)
{
	char *p;
	EfiVar *v;

	p = io_mgets(io, "%s,;{}");

	for (v = type->list; v; v = v->next)
	{
		if	(patcmp(p, v->name, NULL))
		{
			EfiVar *var;
			ConstructEntry *entry;

			if	(name)
			{
				var = NewVar(v->type, NULL, v->dim);
				var->name = name;
			}
			else	var = NewVar(v->type, v->name, v->dim);

			entry = add_var(par, var, name ? 1 : 0);

			if	(entry)
			{
				entry->base = rd_refer(v);
				entry->idx = idx;
			}

			if	(name)	break;
		}
	}

	memfree(p);
}

static void add_member_list (IO *io, ConstructPar *par, char *name, char *var)
{
	int i;

	for (i = 0; i < par->dim; i++)
	{
		if	(mstrcmp(par->var[i]->name, var) == 0)
		{
			memfree(var);
			add_member(io, par, par->var[i]->type, name, i);
			return;
		}
	}

	dbg_error("edb", ERR_VAR, "s", var);
}

static int parse_expr (IO *io, ConstructPar *par, int delim)
{
	int c;
	EfiType *type;
	EfiParseDef *parse;
	EfiObj *obj;
	char *name;

	c = io_eat(io, "%s,;");

	if	(c == delim)
	{
		io_getc(io);
		return 0;
	}

	name = io_getname(io);
	type = NULL;

	if	(name)
	{
		if	((type = GetType(name)))
		{
			memfree(name);
			name = io_getname(io);
		}
		else if	((parse = GetParseDef(name)))
		{
			obj = (*parse->func)(io, parse->data);
			obj = EvalObj(obj, &Type_type);
			type = obj ? Val_type(obj->data) : NULL;
			UnrefObj(obj);
			memfree(name);
			name = io_getname(io);
		}

		c = io_eat(io, "%s");
	}

	c = io_getc(io);

	if	(c == '?')
	{
		if	(par->obj)
		{
			list_var(ioerr, NULL, par->obj);
		}
		else
		{
			int i;

			for (i = 0; i < par->dim; i++)
				list_var(ioerr, par->var[i]->name,
					par->var[i]->obj);
		}

		exit(EXIT_SUCCESS);
	}

	if	(c == '{')
	{
		EfiFunc *func = cf_create();
		cf_set(func, name, io, '}');
		memfree(name);

		if	(func)
		{
			ConstructEntry *entry = add_var(par,
				GetStructEntry(io, func->type), 1);

			if	(entry)
				entry->func = rd_refer(func);
		}

		return 1;
	}

	if	(c == '=')
	{
		add_expr(par, type, name, Parse_term(io, OpPrior_Comma));
	}
	else if	(type)
	{
		EfiVar *var = NewVar(type, NULL, 0);
		var->name = name;
		var->member = StructMember;
		var->offset = 0;
		add_var(par, var, 1);
	}
	else if	(c == ':')
	{
		char *p;

		if	(par->obj)
		{
			add_member(io, par, par->obj->type, name, 0);
		}
		else
		{
			p = io_getname(io);
			c = io_getc(io);
			add_member_list(io, par, name, p);
		}
		
		return 1;
	}
	else if	(c == '.')
	{
		if	(par->obj)
			dbg_error("edb", ERR_VAR, "s", name);

		add_member_list(io, par, NULL, name);
	}
	else if	(!name)
	{
		if	(c == ',' || c == ';')
			return 1;

		io_ungetc(c, io);
		add_member(io, par, par->obj->type, NULL, 0);
		return 1;
	}
	else
	{
		if	(par->obj)
		{
			EfiVar *v;

			for (v = par->obj->type->list; v; v = v->next)
			{
				if	(mstrcmp(name, v->name) == 0)
				{
					EfiVar *var;
					ConstructEntry *entry;

					var = NewVar(v->type, NULL, v->dim);
					var->name = name;
					entry = add_var(par, var, 1);

					if	(entry)
					{
						entry->base = rd_refer(v);
						entry->idx = 0;
					}

					return 1;
				}
			}
		}

		obj = NewPtrObj(&Type_name, mstrcpy(name));
		add_expr(par, NULL, name, obj);
	}

	return 1;
}

static int test_parenthesis (const char *def)
{
	char *p;

	if	(def[0] != '{')	return 0;

	p = strrchr(def, '}');

	if	(p == NULL)	return 0;

	while (*(++p))
		if (!isspace(*p)) return 0;
	
	return 1;
}


EfiFunc *ConstructFunc (EfiType *base, const char *name, const char *def)
{
	IO *io;
	EfiFunc *func;

	if	(!base || !def)
		return NULL;

	while (isspace(*def))
		def++;

	func = cf_create();
	func->dim = 1;
	func->arg = memalloc(sizeof *func->arg);
	func->arg->name = NULL;
	func->arg->type = base;
	func->arg->defval = ConstObj(base, NULL);
	func->arg->lval = 0;
	func->arg->cnst = 0;
	func->arg->promote = 0;
	func->arg->nokonv = 0;

	if	(test_parenthesis(def))
	{
		io = io_cstr(def + 1);
		cf_set(func, name, io, '}');
	}
	else
	{
		io = io_cstr(def);
		cf_set(func, name, io, EOF);
	}

	io_close(io);
	return func;
}

static void cf_set (EfiFunc *func, const char *name, IO *io, int delim)
{
	ConstructPar *par;

	par = Construct_alloc(func);
	PushVarTab(RefVarTab(par->tab), RefObj(par->obj));

	while (parse_expr(io, par, delim))
		;

	PopVarTab();
	func->type = MakeStruct(mstrcpy(name), NULL, RefVarList(par->vlist));
	func->par = par;
	AddFunc(func);
#if	SHOW_EVAL
	show_eval(par);
#endif
}


EfiObj *PFunc_construct (IO *io, void *data)
{
	EfiFunc *func;
	EfiFuncArg *arg;
	size_t dim;
	char *name;
	int c;

	name = io_getname(io);
	c = io_eat(io, "%s");

	if	(c == ':')
	{
		io_getc(io);
		dim = GetFuncArg(io, &arg, '{');
	}
	else if	(c != '{')
	{
		io_error(io, "[efmain:156]", "m", name);
		return NULL;
	}
	else
	{
		io_getc(io);
		arg = NULL;
		dim = 0;
	}

	func = cf_create();
	func->arg = arg;
	func->dim = dim;
	cf_set(func, name, io, '}');
	return type2Obj(Parse_type(io, func->type));
}
