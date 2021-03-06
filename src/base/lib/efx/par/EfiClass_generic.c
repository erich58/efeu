/*
Klassifikation mit switch und Testketten

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

#include <EFEU/EfiClass.h>

#define	DELIM	" \t:"
#define	cmd_key(x)	((x) == '!' || (x) == '@')

#define	SHOW_ENTRY	0

typedef struct SParEntry SParEntry;

struct SParEntry {
	int (*eval) (SParEntry *entry, const EfiObj *base);
	SParEntry *next;
	SParEntry *sub;
	EfiObj *base;
	EfiObj *expr;
	int rval;
};

static int eval_true (SParEntry *entry, const EfiObj *base)
{
	return 1;
}

static int eval_case (SParEntry *entry, const EfiObj *base)
{
	EfiObj *obj = EvalObj(BinaryTerm("==", RefObj(base),
		RefObj(entry->expr)), NULL);
	return Obj2bool(obj);
}

static int eval_test (SParEntry *entry, const EfiObj *base)
{
	return Obj2bool(RefObj(entry->expr));
}

static int eval_expr (SParEntry *entry, const EfiObj *base)
{
	EvalObj(RefObj(entry->expr), NULL);
	return 0;
}


static SParEntry *new_entry (void)
{
	SParEntry *entry = memalloc(sizeof *entry);
	return entry;
}

static void del_entry (SParEntry *entry)
{
	if	(entry)
	{
		UnrefObj(entry->base);
		UnrefObj(entry->expr);
		del_entry(entry->next);
		del_entry(entry->sub);
		memfree(entry);
	}
}

static int test_entry (SParEntry *entry, const EfiObj *base, const EfiObj *obj)
{
	for (; entry; entry = entry->next)
	{
		if	(entry->eval && entry->eval(entry, obj))
			break;
	}

	for (; entry; entry = entry->next)
	{
		if	(entry->sub)
		{
			EfiObj *x = EvalObj(RefObj(entry->base), NULL);
			int rval = test_entry(entry->sub, base, x ? x : base);
			UnrefObj(x);

			if	(rval)
				return rval;
		}

		if	(entry->rval)
			return entry->rval;
	}
	
	return 0;
}

#if	SHOW_ENTRY
static void show_entry (IO *out, int depth, SParEntry *entry)
{
	for (; entry; entry = entry->next)
	{
		io_nputc('\t', out, depth);
		io_xprintf(out, "%d\t", entry->rval);

		if	(!entry->eval)
			io_puts("-", out);
		else if	(entry->eval == eval_true)
			io_puts("default", out);
		else if	(entry->eval == eval_case)
			io_puts("case", out);
		else if	(entry->eval == eval_test)
			io_puts("test", out);
		else if	(entry->eval == eval_expr)
			io_puts("eval", out);
		else	io_puts("?", out);

		if	(entry->base)
			io_printf(out, "\tbase=%p", entry->base);

		if	(entry->expr)
			io_printf(out, "\texpr=%p", entry->expr);

		io_putc('\n', out);
		show_entry(out, depth + 1, entry->sub);
	}
}
#endif

typedef struct {
	REFVAR;
	SParEntry *entry;
	EfiVarTab *tab;
} SPar;

typedef struct {
	IO *in;
	EfiType *type;
	char *arg[2];
	int code;
} SDef;

static void sdef_beg (SDef *def)
{
	memfree(def->arg[0]);
	def->arg[0] = NULL;
	memfree(def->arg[1]);
	def->arg[1] = NULL;
}

static void sdef_key (SDef *def, char *key)
{
	memfree(def->arg[0]);
	def->arg[0] = def->arg[1];
	def->arg[1] = mstrcpy(key);
}

static char *sdef_psub (SDef *def, const char *fmt)
{
	return mpsubvec(fmt, 2, def->arg);
}

static void SPar_free (void *data)
{
	SPar *par = data;
	del_entry(par->entry);
	DelVarTab(par->tab);
	memfree(par);
}

static RefType SPar_reftype = REFTYPE_INIT("SwitchPar", NULL, SPar_free);

static int par_update (const EfiObj *obj, void *opaque_par)
{
	SPar *par = opaque_par;
	int rval;

	PushVarTab(RefVarTab(obj->type->vtab), RefObj(obj));
	PushVarTab(RefVarTab(par->tab), NULL);
	PushVarTab(NULL, NULL);
	rval = test_entry(par->entry, obj, obj);
	PopVarTab();
	PopVarTab();
	PopVarTab();
	return rval;
}

static char *get_line (IO *io)
{
	static STRBUF(buf, 0);
	char *p;
	
	if	((io_eat(io, "%s") == EOF))
		return NULL;

	do
	{
		p = io_mgetline(io, &buf, "\n;");
		io_getc(io);
	}
	while	(*p == 0);

	return p;
}

static char *next_arg (char *p, char **ptr)
{
	int n, k;
	int quote;

	if	(p[0] == '\\' && p[1])
		p++;

	quote = 0;

	for (k = n = 0; p[n]; n++)
	{
		if	(!quote && strchr(DELIM, p[n]))
		{
			do	n++;
			while	(p[n] && strchr(DELIM, p[n]));

			break;
		}

		if	(p[n] == '"')
		{
			quote = !quote;
			continue;
		}
		else if	(p[n] == '\\' && p[n+1])
		{
			n++;

			switch (p[n])
			{
			case 'n':	p[k++] = '\n'; break;
			case 't':	p[k++] = '\t'; break;
			default:	p[k++] = p[n]; break;
			}
		}
		else	p[k++] = p[n];
	}

	p[k] = 0;
	*ptr = p + n;
	return p;
}

static int make_label (SDef *sdef, char *def)
{
	if	(def && *def)
	{
		char *p = next_arg(def, &def);
		AddEnumKey(sdef->type,
			sdef_psub(sdef, p),
			sdef_psub(sdef, def),
			++sdef->code);
		return sdef->code;
	}
	else	return 0;
}

static char *test_key (const char *key, char *def)
{
	while (*key)
	{
		if	(*def != *key)
			return NULL;

		key++;
		def++;
	}

	if	(!*def)
		return def;

	if	(strchr(DELIM, *def))
	{
		do	def++;
		while	(*def && strchr(DELIM, *def));

		return def;
	}
	
	return NULL;
}


static SParEntry *parse_test (SDef *def, int stat, const char *opt,
	int (*eval) (SParEntry *entry, const EfiObj *base))
{
	SParEntry *entry, **ptr;
	char *key, *p, *arg;

	entry = NULL;
	ptr = &entry;
	sdef_beg(def);

	while ((p = get_line(def->in)))
	{
		if	(cmd_key(*p))
		{
			p++;

			if	(test_key("end", p))
			{
				break;
			}
			else if	((arg = test_key("case", p)))
			{
				SParEntry *x = new_entry();
				x->eval = stat ? NULL : eval_true;
				stat = 1;
				x->base = arg ? strterm(arg) : NULL;
				x->sub = parse_test(def, 1, NULL, eval_case);
				*ptr = x;
				ptr = &(*ptr)->next;
			}
			else if	((arg = test_key("test", p)))
			{
				SParEntry *x = new_entry();
				x->eval = stat ? NULL : eval_true;
				stat = 1;
				arg = *arg ? mstrcpy(arg) : NULL;
				x->sub = parse_test(def, 1, arg, eval_test);
				memfree(arg);
				*ptr = x;
				ptr = &(*ptr)->next;
			}
			else if	((arg = test_key("other", p)))
			{
				*ptr = new_entry();
				(*ptr)->eval = eval_true;
				(*ptr)->rval = make_label(def, arg);
				ptr = &(*ptr)->next;
			}
			else if	((arg = test_key("eval", p)))
			{
				*ptr = new_entry();
				(*ptr)->eval = eval_expr;
				(*ptr)->expr = strterm(arg);
				ptr = &(*ptr)->next;
			}
			else
			{
				io_xprintf(ioerr, "!%s\n", p);
			}

			continue;
		}

		key = next_arg(p, &p);
		*ptr = new_entry();

		if	(mstrcmp("*", key) == 0)
		{
			sdef_key(def, NULL);
			(*ptr)->eval = eval_true;
		}
		else
		{
			sdef_key(def, key);
			(*ptr)->eval = eval;

			if	(opt)
			{
				char *cmd = sdef_psub(def, opt);
				(*ptr)->expr = strterm(cmd);
				memfree(cmd);
			}
			else	(*ptr)->expr = strterm(key);
		}

		(*ptr)->rval = make_label(def, p);
		ptr = &(*ptr)->next;
	}

	return entry;
}

static void par_create (EfiClassArg *def, const EfiType *type,
	const char *opt, const char *arg, void *eval)
{
	SDef sdef;
	SPar *par;

	par = memalloc(sizeof *par);

	sdef.in = io_cstr(arg);
 	sdef.type = NewEnumType(NULL, 1);
	sdef.arg[0] = NULL;
	sdef.arg[1] = NULL;
	sdef.code = 0;

	PushVarTab(NULL, NULL);
	VarTab_xadd(NULL, "last", NULL,
		LvalObj(&Lval_ptr, &Type_str, sdef.arg));
	VarTab_xadd(NULL, "key", NULL,
		LvalObj(&Lval_ptr, &Type_str, sdef.arg + 1));

	PushVarTab(NULL, NULL);
	par->entry = parse_test(&sdef, 0, opt, eval);
	par->tab = RefVarTab(LocalVar);
	PopVarTab();

	io_close(sdef.in);
	PopVarTab();
 	def->type = AddEnumType(sdef.type);

#if	SHOW_ENTRY
	show_entry(ioerr, 0, par->entry);
	io_puts("-----\n", ioerr);
#endif

	def->update = par_update;
	def->par = rd_init(&SPar_reftype, par);
}

EfiClass EfiClass_generic = EFI_CLASS(NULL,
	"generic", "[expr]=case label desc;...", par_create, NULL,
	"generic class construction"
);

EfiClass EfiClass_switch = EFI_CLASS(NULL,
	"switch", "[fmt]=key label desc;...", par_create, eval_case,
	"switch expression"
);

EfiClass EfiClass_test = EFI_CLASS(NULL,
	"test", "[fmt]=key label desc;...", par_create, eval_test,
	"test expression"
);
