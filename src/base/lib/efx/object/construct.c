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
#include <EFEU/EfiClass.h>
#include <EFEU/MatchPar.h>
#include <EFEU/ioctrl.h>
#include <EFEU/preproc.h>
#include <EFEU/EDB.h>
#include <ctype.h>

#define	PAR_DEBUG	0
#define	SHOW_EVAL	0

#define	ERR_TYP	"[edb:typ]$!: unknown datatype $1.\n"
#define ERR_USE	"[edb:use]$!: name $1 arready used.\n"
#define ERR_VAR	"[edb:var]$!: unknown variable $1.\n"
#define ERR_NAME	"$!: missing variable name.\n"
#define ERR_CHAR	"$!: unexpected character $1.\n"

/*	Selektionseinträge
*/

typedef struct ConstructEntry ConstructEntry;

struct ConstructEntry {
	ConstructEntry *next;
	EfiStruct *var;
	EfiFunc *func;
	EfiObj *obj;
	EfiStruct *base;
	int idx;
};

static ALLOCTAB(entry_tab, "ConstructEntry", 0, sizeof(ConstructEntry));

static ConstructEntry *ConstructEntry_alloc (EfiStruct *var)
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
	size_t idx;
	VarTabEntry **var;
	EfiVarTab *tab;
	EfiObj *obj;
	EfiStruct *vlist;
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
	par->idx = 0;
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
					func->arg[i].desc,
					farg_obj(func->arg + i));

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
	else if	(func->scope)
	{
		par->tab = RefVarTab(func->scope);
	}
	
	par->vlist = NULL;
	par->root = NULL;
	par->ptr = &par->root;
	func->par = par;
	return par;
}

#if	PAR_DEBUG
static void par_info (ConstructPar *par, const char *pfx)
{
	if	(par->obj)
		io_xprintf(ioerr, "%s: obj.refcount: %d\n", pfx,
			par->obj->refcount);
}
#endif

static void Construct_clean (void *opaque_par)
{
	ConstructPar *par = opaque_par;
	DelEfiStruct(par->vlist);
	DelVarTab(par->tab);
#if	PAR_DEBUG
	par_info(par, "clean");
#endif
	UnrefObj(par->obj);
	ConstructEntry_clean(par->root);
	memfree(par);
}

static ConstructEntry *Construct_next (ConstructPar *par, EfiStruct *var)
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
		io_xprintf(ioerr, "%s: ", entry->var->name);

		if	(entry->func)
		{
			io_xprintf(ioerr, "func\n");
		}
		else if	(entry->obj)
		{
			if	(entry->obj->type->eval)
				io_xprintf(ioerr, "expr\n");
			else if	(entry->obj->lval)
				io_xprintf(ioerr, "lval\n");
			else	io_xprintf(ioerr, "const\n");
		}
		else if	(entry->base)
		{
			io_xprintf(ioerr, "copy(%d)\n", entry->idx);
		}
		else	io_xprintf(ioerr, "defval\n");
	}
}
#endif

/*	Funktionsverwaltung
*/

static EfiFunc *cf_create (const EfiObj *obj)
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

	if	(obj)
	{
		func->dim = 1;
		func->arg = memalloc(sizeof *func->arg);
		func->arg->name = NULL;
		func->arg->type = obj->type;
		func->arg->defval = RefObj(obj);
		func->arg->lval = 0;
		func->arg->cnst = 0;
		func->arg->promote = 0;
		func->arg->noconv = 0;
	}

	return func;
}

/*	Parameter zusammenstellen
*/

static void cf_set (EfiFunc *func, const char *name, IO *io, int delim);

static ConstructEntry *add_var (ConstructPar *par, EfiStruct *var, int flag)
{
	EfiStruct **p;

	if	(!var)	return NULL;

	for (p = &par->vlist; *p; p = &(*p)->next)
	{
		if	(mstrcmp(var->name, (*p)->name) == 0)
		{
			if	(flag)
				log_error(edb_err, ERR_USE, "s", var->name);

			rd_deref(var);
			return NULL;
		}
	}

	*p = var;
	return Construct_next(par, var);
}

static void add_expr (ConstructPar *par, EfiType *type,
	char *name, int flag, EfiObj *obj)
{
	EfiObj *cobj = EvalObj(RefObj(obj), type);

	if	(cobj)
	{
		EfiStruct *var;
		ConstructEntry *entry;

		var = NewEfiStruct(cobj->type, NULL, 0);
		var->name = name;
		var->offset = 0;
		var->defval = cobj;
		entry = add_var(par, var, flag);

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


static EfiObj *member_obj (EfiObj *obj, EfiStruct *st)
{
	if	(!obj->lval || st->member || st->dim)
	{
		EfiName name;
		name.obj = obj;
		name.name = mstrcpy(st->name);
		return NewObj(&Type_mname, &name);
	}
	
	return LvalObj(&Lval_obj, st->type, obj,
		(char *) obj->data + st->offset);
}

static size_t match_dim_base (EfiObj *base)
{
	EfiStruct *st;
	size_t n;

	for (n = 0, st = base->type->list; st; st = st->next, n++)
		;

	return n;
}

static size_t match_dim (ConstructPar *par)
{
	if	(par->obj)	return match_dim_base(par->obj);
	else if	(par->dim)	return par->dim;
	else if	(par->tab)	return par->tab->tab.used;
	else			return 0;
}

static void add_select_base (ConstructPar *par, EfiObj *base, char *name,
	MatchPar *mp)
{
	EfiStruct *st;
	size_t n;

	for (n = 1, st = base->type->list; st; st = st->next, n++)
	{
		if	(MatchPar_exec(mp, st->name, n))
		{
			add_expr(par, NULL,
				name ? name : mstrcpy(st->name),
				name ? 1 : 0, member_obj(base, st));
		}
	}
}

static void add_select (ConstructPar *par, char *name, MatchPar *mp)
{
	if	(par->obj)
	{
		add_select_base(par, par->obj, name, mp);
		return;
	}
	else if	(par->dim)
	{
		size_t n;

		for (n = 0; n < par->dim; n++)
		{
			VarTabEntry *var = par->var[n];

			if	(MatchPar_exec(mp, var->name, n + 1))
			{
				add_expr(par, NULL,
					name ? name : mstrcpy(var->name),
					name ? 1 : 0, RefObj(var->obj));
			}
		}
	}
	else if	(par->tab)
	{
		VarTabEntry *var = par->tab->tab.data;
		size_t n;

		for (n = 1; n <= par->tab->tab.used; n++, var++)
		{
			if	(MatchPar_exec(mp, var->name, n))
			{
				add_expr(par, NULL,
					name ? name : mstrcpy(var->name),
					name ? 1 : 0, RefObj(var->obj));
			}
		}
	}
}


static EfiObj *get_member (EfiObj *base, const char *member)
{
	EfiName name;

	if	(base->lval)
	{
		EfiStruct *st;

		for (st = base->type->list; st; st = st->next)
		{
			if	(mstrcmp(st->name, member) != 0)
				continue;

			if	(st->member || st->dim)
				break;

			return LvalObj(&Lval_obj, st->type, base,
				(char *) base->data + st->offset);
		}

	}

	name.obj = base;
	name.name = mstrcpy(member);
	return NewObj(&Type_mname, &name);
}

static EfiObj *get_base (ConstructPar *par, const char *name)
{
	EfiObj *base;

	if	(par->obj)
	{
		base = get_member(RefObj(par->obj), name);
	}
	else if	(par->tab)
	{
		base = GetVar(par->tab, name, NULL);
	}
	else
	{
		base = NewPtrObj(&Type_name, mstrcpy(name));
	}

	return base;
}

static EfiObj *parse_class (IO *io, EfiObj *base)
{
	while (io_eat(io, "%s") == '{')
	{
		char *p = getstring(io);
		base = EfiClassExpr(base, p);
		memfree(p);
	}

	return base;
}

static int parse_expr (IO *io, ConstructPar *par, int delim);

static void add_desc (IO *io, ConstructPar *par)
{
	EfiStruct *st;
	char *p = NULL;

	if	(!par->vlist)
	{
		io_ctrl(io, IOPP_COMMENT, NULL);
		return;
	}

	io_ctrl(io, IOPP_COMMENT, &p);

	if	(p)
	{
		mtrim(p);

		for (st = par->vlist; st->next; st = st->next)
			;

		memfree(st->desc);
		st->desc = p;
	}
}

static int parse_list (IO *io, ConstructPar *par, int delim)
{
	io_getc(io);

	while (parse_expr(io, par, delim))
		;

	return 1;
}

static int parse_expr (IO *io, ConstructPar *par, int delim)
{
	int c;
	EfiType *type;
	EfiParseDef *parse;
	EfiObj *obj;
	MatchPar *mp;
	char *name;

	c = io_eat(io, "%s,;");
	add_desc(io, par);

	if	(c == EOF)
		return 0;

	if	(c == delim)
	{
		io_getc(io);
		return 0;
	}

	if	(c == '?')
	{
		if	(par->obj)
		{
			list_var(ioerr, NULL, par->obj);
		}
		else if	(par->tab)
		{
			ShowVarTab(ioerr, "var", par->tab);
			io_putc('\n', ioerr);
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
		return parse_list(io, par, '}');

	if	(c == '(')
		return parse_list(io, par, ')');

	if	(c == '*')
	{
		io_getc(io);
		mp = MatchPar_create(NULL, match_dim(par));
		add_select(par, NULL, mp);
		rd_deref(mp);
		return 1;
	}
	else if	(c == '[')
	{
		io_getc(io);
		mp = MatchPar_scan(io, ']', match_dim(par));
		add_select(par, NULL, mp);
		rd_deref(mp);
		return 1;
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

		if	(!name)
			log_error(edb_err, ERR_NAME, NULL);
	}
	else	log_error(edb_err, ERR_CHAR, "c", c);

	c = io_getc(io);

	if	(c == delim || c == EOF || c == ',' || c == ';')
	{
		if	(type)
		{
			EfiStruct *var = NewEfiStruct(type, NULL, 0);
			var->name = name;
			var->offset = 0;
			add_var(par, var, 1);
		}
		else	add_expr(par, NULL, name, 1, get_base(par, name));

		return (c == ',' || c == ';');
	}

	if	(c == '{')
	{
		EfiObj *base = get_base(par, name);
		io_ungetc(c, io);
		base = parse_class(io, base);
		add_expr(par, type, name, 1, base);
		return 1;
	}
	else if	(c == '.')
	{
		EfiObj *base;

		base = get_base(par, name);
		memfree(name);
		name = io_getname(io);
		c = io_getc(io);

		while (name && c == '.')
		{
			base = get_member(base, name);
			memfree(name);
			name = io_getname(io);
			c = io_getc(io);
		}

		if	(name)
		{
			io_ungetc(c, io);
			base = get_member(base, name);
			base = parse_class(io, base);
			add_expr(par, NULL, name, 1, base);
		}
		else if	(c == '*')
		{
			mp = MatchPar_create(NULL, match_dim_base(base));
			add_select_base(par, base, NULL, mp);
			rd_deref(mp);
		}
		else if	(c == '[')
		{
			mp = MatchPar_scan(io, ']', match_dim_base(base));
			add_select_base(par, base, NULL, mp);
			rd_deref(mp);
		}
		else if	(c == '?')
		{
			list_var(ioerr, NULL, base);
			exit(EXIT_SUCCESS);
		}
		else	log_error(edb_err, ERR_CHAR, "c", c);

		return 1;
	}

	if	(c == '=')
	{
		c = io_eat(io, "%s");

		if	(c == '{')
		{
			EfiFunc *func;
			EfiStruct *st;
			ConstructEntry *entry;
			
			io_getc(io);
			func = cf_create(par->obj);
  			cf_set(func, NULL, io, '}');
			st = NewEfiStruct(func->type, name, 0);
			st->offset = 0;
			entry = add_var(par, st, 1);

			if	(entry)
				entry->func = rd_refer(func);

			return 1;
		}
		else
		{
			obj = Parse_term(io, OpPrior_Comma);
			obj = parse_class(io, obj);
			add_expr(par, type, name, 1, obj);
			return 1;
		}
	}
	else	log_error(edb_err, ERR_CHAR, "c", c);

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

static void cf_set (EfiFunc *func, const char *name, IO *io, int delim)
{
	ConstructPar *par;

	par = Construct_alloc(func);
	PushVarTab(RefVarTab(par->tab), RefObj(par->obj));
#if	PAR_DEBUG
	par_info(par, "beg");
#endif

	while (parse_expr(io, par, delim))
		;

#if	PAR_DEBUG
	par_info(par, "end");
#endif
	PopVarTab();
	func->type = MakeStruct(mstrcpy(name), NULL, RefEfiStruct(par->vlist));
	func->par = par;
#if	SHOW_EVAL
	show_eval(par);
#endif
}


EfiFunc *ConstructObjFunc (const char *name, const char *def, EfiObj *obj)
{
	IO *io;
	EfiFunc *func;

	if	(!obj || !def)
		return NULL;

	while (isspace(*def))
		def++;

	func = cf_create(obj);

	if	(test_parenthesis(def))
	{
		io = io_cmdpreproc(io_cstr(def + 1));
		cf_set(func, name, io, '}');
	}
	else
	{
		io = io_cmdpreproc(io_cstr(def));
		cf_set(func, name, io, EOF);
	}

	io_close(io);
	return func;
}

EfiFunc *ConstructFunc (const char *name, const char *def, EfiVarTab *tab)
{
	IO *io;
	EfiFunc *func;
	
	func = cf_create(NULL);
	func->scope = RefVarTab(tab);
	
	if	(test_parenthesis(def))
	{
		io = io_cmdpreproc(io_cstr(def + 1));
		cf_set(func, name, io, '}');
	}
	else
	{
		io = io_cmdpreproc(io_cstr(def));
		cf_set(func, name, io, EOF);
	}

	io_close(io);
	return func;
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

	func = cf_create(NULL);
	func->arg = arg;
	func->dim = dim;
	cf_set(func, name, io, '}');
	AddFunc(func);
	return type2Obj(Parse_type(io, func->type));
}
