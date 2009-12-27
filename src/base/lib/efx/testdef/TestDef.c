/*
:*:	test definition
:de:	Testdefinition

$Copyright (C) 2005 Erich Frühstück
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

#include <EFEU/TestDef.h>
#include <EFEU/parsearg.h>
#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
#include <EFEU/konvobj.h>
#include <ctype.h>

#define	ID	"TestDef"

static ALLOCTAB(entry_tab, "TestDefEntry", 0, sizeof(TestDefEntry));

static TestDefEntry *test_entry (EfiType *type, const char *name,
	const char *opt, const char *arg);

typedef struct {
	EfiObj *base;
	EfiStruct *st;
	VarTabEntry *var;
	TestDefEntry *entry;
} MDEF;

static void put_entry (TestDefEntry *p, IO *io)
{
	io_xprintf(io, "%d:%d*%d %s", p->offset, p->dim,
		p->type->size, p->type->name);

	if	(p->clean == rd_clean)
	{
		char *x = rd_ident(p->par);

		io_puts(" {", io);
		io_puts(x, io);
		io_puts("}", io);
		memfree(x);
	}
	else	io_xprintf(io, " {%p}", p->par);
}

static char *test_ident (const void *ptr)
{
	const TestDef *test = ptr;

	if	(test->par)
		return msprintf("TestParDef(%p)", test->par);

	if	(test->list)
	{
		TestDefEntry *p;
		StrBuf *sb;
		IO *io;

		sb = sb_acquire();
		io = io_strbuf(sb);
		io_puts(test->type->name, io);

		for (p = test->list; p; p = p->next)
		{
			io_putc(' ', io);
			put_entry(p, io);
		}

		io_close(io);
		return sb_cpyrelease(sb);
	}

	return NULL;
}

static void entry_clean (TestDefEntry *entry)
{
	if	(entry)
	{
		entry_clean(entry->next);

		if (entry->clean) entry->clean(entry->par);

		del_data(&entry_tab, entry);
	}
}

static void test_clean (void *data)
{
	TestDef *test = data;
	entry_clean(test->list);
	memfree(test);
}

static RefType TestDef_reftype = REFTYPE_INIT(ID, test_ident, test_clean);

TestDef *test_create (TestParDef *def, TestDefFunc test,
	const char *opt, const char *arg)
{
	TestDef *par = memalloc(sizeof *par);
	par->type = NULL;
	par->list = NULL;

	par->par = def;
	par->test = test;
	par->data = def && def->create ?
		def->create(opt, arg, def->data) : NULL;
	return rd_init(&TestDef_reftype, par);
}

static char *name_cmp (const char *a, const char *b)
{
	while (*a && *b)
	{
		if	(*a != *b)	return NULL;

		a++;
		b++;
	}

	if	(*a)		return NULL;
	if	(*b == 0)	return (char *) b;
	if	(*b == '.')	return (char *) b + 1;

	return NULL;
}

static EfiStruct *get_struct (EfiType *type, const char *def, char **ptr)
{
	EfiStruct *st;

	for (st = type->list; st; st = st->next)
		if	((*ptr = name_cmp(st->name, def)))
			return st;

	*ptr = NULL;
	return NULL;
}

static int test_true (TestDefEntry *entry, void *data)
{
	return 1;
}

static int test_list (TestDefEntry *entry, void *data)
{
	EfiObjList *list;

	for (list = entry->par; list; list = list->next)
		if (memcmp(list->obj->data, data, entry->type->size) == 0)
			return 1;

	return 0;
}

static void clean_list (void *par)
{
	DelObjList(par);
}

static void set_test (TestDefEntry *entry, const char *opt, const char *arg)
{
	if	(!opt)
	{
		if	(entry->type->flags & TYPE_ENUM)
			opt = "label";
		else	opt = "list";
	}

	if	(mstrcmp(opt, "list") == 0)
	{
		IO *io = io_cstr(arg);
		EfiObjList *list;

		entry->clean = clean_list;
		entry->par = Parse_list(io, EOF);
		entry->tst = entry->par ? test_list : test_true;
		io_close(io);

		for (list = entry->par; list; list = list->next)
		{
			EfiObj *x = list->obj;
			list->obj = KonvObj(x, entry->type);

			if	(!list->obj)
			{
				list->obj = ConstObj(entry->type,
					entry->type->defval);
			}
		}
	}
	else if	(mstrcmp(opt, "label") == 0)
	{
		EfiObjList *list, **ptr;
		EfiObj *obj;
		char *p, *val;

		list = NULL;
		ptr = &list;

		for (; arg; arg = p)
		{
			val = mstrcut(arg, &p, ",", 1);
			obj = KonvObj(str2Obj(val), entry->type);

			if	(obj)
			{
				*ptr = NewObjList(obj);
				ptr = &(*ptr)->next;
			}
		}

		entry->tst = list ? test_list : test_true;
		entry->clean = clean_list;
		entry->par = list;
	}
}

static void mdef_clean (void *ptr)
{
	MDEF *mdef = ptr;
	UnrefObj(mdef->base);
	memfree(mdef);
}

static int test_var (TestDefEntry *entry, void *data)
{
	MDEF *mdef;
	EfiObj *x;
	int s;

	mdef = entry->par;
	mdef->base->data = data;
	x = mdef->var->get(mdef->base, mdef->var->data);
	s = mdef->entry->tst(mdef->entry,
		(char *) x->data + mdef->entry->offset);
	UnrefObj(x);
	return s;
}

static int test_member (TestDefEntry *entry, void *data)
{
	MDEF *mdef;
	EfiObj *x;
	int s;

	mdef = entry->par;
	mdef->base->data = data;
	x = mdef->st->member(mdef->st, mdef->base);
	s = mdef->entry->tst(mdef->entry,
		(char *) x->data + mdef->entry->offset);
	UnrefObj(x);
	return s;
}


static void set_var (TestDefEntry *entry, const char *name,
	const char *opt, const char *arg)
{
	VarTabEntry *var;
	MDEF *mdef;
	char *p;
	
	if	((p = strchr(name, '.')))
	{
		char *base = mstrncpy(name, p - name);
		var = VarTab_get (entry->type->vtab, base);
		memfree(base);
		name = p + 1;
	}
	else
	{
		var = VarTab_get (entry->type->vtab, name);
		name = NULL;
	}

	if	(!var || !var->get)	return;

	mdef = memalloc(sizeof *mdef);
	mdef->base = LvalObj(&Lval_ptr, entry->type, NULL);
	mdef->st = NULL;
	mdef->var = var;
	mdef->entry = test_entry(var->type, name, opt, arg);

	entry->par = mdef;
	entry->tst = test_var;
	entry->clean = mdef_clean;
}

static void set_member (TestDefEntry *entry, EfiStruct *st,
	const char *name, const char *opt, const char *arg)
{
	MDEF *mdef = memalloc(sizeof *mdef);

	mdef->base = LvalObj(&Lval_ptr, entry->type, NULL);
	mdef->st = st;
	mdef->var = NULL;
	mdef->entry = test_entry(st->type, name, opt, arg);

	entry->par = mdef;
	entry->tst = test_member;
	entry->clean = mdef_clean;
}

static TestDefEntry *test_entry (EfiType *type, const char *name,
	const char *opt, const char *arg)
{
	TestDefEntry *entry;
	EfiStruct *st;
	char *p;

	entry = new_data(&entry_tab);
	entry->next = NULL;
	entry->type = type;
	entry->offset = 0;
	entry->dim = 1;
	entry->tst = test_true;
	entry->par = NULL;
	entry->clean = NULL;

	while (name && *name)
	{
		if	(*name == '.')
		{
			name++;
			continue;
		}

		st = get_struct(entry->type, name, &p);

		if	(!st)
		{
			set_var(entry, name, opt, arg);
			return entry;
		}

		if	(st->dim)
		{
			return NULL;
		}
		else if	(st->member)
		{
			set_member(entry, st, p, opt, arg);
			return entry;
		}
		else
		{
			entry->type = st->type;
			entry->offset += st->offset;
		}

		name = p;
	}

	set_test(entry, opt, arg);
	return entry;
}

TestDef *TestDef_create (EfiType *type, const char *def)
{
	TestDef *tst;
	TestDefEntry **ptr;
	char *p;
	AssignArg *x; 

	tst = memalloc(sizeof *tst);
	tst->type = type;
	tst->list = NULL;
	tst->par = NULL;
	tst->test = NULL;
	tst->data = NULL;

	ptr = &tst->list;

	while ((x = assignarg(def, &p, " \t\n")))
	{
		if ((*ptr = test_entry(type, x->name, x->opt, x->arg)))
			ptr = &(*ptr)->next;

		memfree(x);
		def = p;
	}

	return rd_init(&TestDef_reftype, tst);
}

int TestDef_test (TestDef *def, const void *data)
{
	TestDefEntry *entry = def ? def->list : NULL;

	while (entry)
	{
		if	(!entry->tst(entry, (char *) data + entry->offset))
			return 0;

		entry = entry->next;
	}

	return 1;
}

int TestDef_obj (TestDef *def, const EfiObj *obj)
{
	if	(!def)	
		return 0;

	if	(def->test)
		return def->test(def->data, obj);

	if	(def->type != obj->type)
	{
		EfiObj *x = EvalObj(RefObj(obj), def->type);
		int r =	TestDef_test(def, x->data);
		UnrefObj(x);
		return r;
	}
	else	return TestDef_test(def, obj->data);
}

