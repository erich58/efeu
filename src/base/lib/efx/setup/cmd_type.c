/*
Datentypspezifische Funktionen

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
#include <EFEU/cmdsetup.h>
#include <EFEU/printobj.h>
#include <EFEU/Info.h>
#include <EFEU/EfiClass.h>
#include <EFEU/Resource.h>
#include <ctype.h>

#define	KEY_TYPE	"Type"
#define LBL_TYPE	":*:data types:de:Datentypen"

/*	Komponenten von Typen
*/

static EfiObj *type_default (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);

	if	(type == NULL)	return NULL;
	else if	(type->defval)	return LvalObj(&Lval_ptr, type, type->defval);
	else			return NewObj(type, NULL);
}

static EfiObj *type_base (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	type = type && type->base ? type->base : &Type_void;
	return ConstObj(&Type_type, &type);
}

static EfiObj *type_recl (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return int2Obj(type ? type->recl : 0);
}

static EfiObj *type_order (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return int2Obj(type ? type->order : 0);
}

static EfiObj *type_dim (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return int2Obj(type ? type->dim : 0);
}

static EfiObj *type_size (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return int2Obj(type ? type->size : 0);
}

static EfiObj *type_flags (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return uint2Obj(type ? type->flags : 0);
}

static EfiObj *type_name (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return str2Obj(type ? mstrcpy(type->name) : NULL);
}

static EfiObj *type_cname (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return str2Obj(type ? mstrcpy(type->cname) : NULL);
}

static EfiObj *type_desc (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return type ? LvalObj(&Lval_ptr, &Type_str, &type->desc) : NULL;
}

static EfiObj *type_create (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return NewPtrObj(&Type_vfunc, type ? rd_refer(type->create) : NULL);
}

static EfiObj *type_var (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	return NewPtrObj(&Type_vtab, type ? rd_refer(type->vtab) : NULL);
}

static EfiObj *type_clist (const EfiObj *obj, void *data)
{
	EfiObjList *list, **ptr;
	EfiType *type;
	
	type = Val_type(obj->data);
	list = NULL;
	ptr = &list;

	if	(type)
	{
		EfiFunc **ftab = type->conv.data;
		size_t n;

		for (n = 0; n < type->conv.used; n++)
		{
#if	0
			*ptr = NewObjList(type2Obj(ftab[n]->type));
#else
			*ptr = NewObjList(NewPtrObj(&Type_func,
				rd_refer(ftab[n])));
#endif
			ptr = &(*ptr)->next;
		}
	}

	return NewPtrObj(&Type_list, list);
}

static EfiObj *type_src (const EfiObj *obj, void *data)
{
	EfiType *type = Val_type(obj->data);
	char *p;

	if	(type == NULL)	return NULL;

	p = type->src ? type->src->cmd : NULL;
	return ConstObj(&Type_str, &p);
}

static EfiMember type_member[] = {
	{ "name", &Type_str, type_name, NULL },
	{ "cname", &Type_str, type_cname, NULL },
	{ "desc", &Type_str, type_desc, NULL },
	{ "default", NULL, type_default, NULL },
	{ "base", NULL, type_base, NULL },
	{ "src", NULL, type_src, NULL },
	{ "order", &Type_int, type_order, NULL },
	{ "dim", &Type_int, type_dim, NULL },
	{ "recl", &Type_int, type_recl, NULL },
	{ "size", &Type_int, type_size, NULL },
	{ "flags", &Type_uint, type_flags, NULL },
	{ "create", &Type_vfunc, type_create, NULL },
	{ "var", &Type_vtab, type_var, NULL },
	{ "convlist", &Type_list, type_clist, NULL },
};

/*	Info-Abfragen
*/

extern unsigned TypeTabChangeCount;
extern NameKeyTab TypeTab;

#if	0
static void show_base (IO *io, const EfiType *type)
{
	ShowType(io, type);

	if	(type->base)
	{
		io_putc('(', io);
		show_base(io, type->base);
		io_putc(')', io);
	}
}
#endif

static void list_name(IO *io, EfiStruct *st)
{
	ShowType(io, st->type);

	if	(st->name == NULL)
		io_puts(" <noname>", io);
	else if	(isalpha(st->name[0]) || st->name[0] == '_')
		io_xprintf(io, " %s", st->name);
	else	io_xprintf(io, " operator%s", st->name);

	if	(st->dim > 1)	io_xprintf(io, "[%d]", st->dim);
}

static void info_type (IO *io, InfoNode *info)
{
	EfiType *type;

	type = info->par;
	io_puts("name = ", io);
	PrintType(io, type, 2);
	io_putc('\n', io);
	io_putc('\n', io);

	if	(type->defval)
	{
		io_puts("default = ", io);

		if	(type->print)
			type->print(type, type->defval, io);
		else	PrintData(io, type, type->defval);

		io_putc('\n', io);
		io_putc('\n', io);
	}

	if	(type->src)
		io_xprintf(io, "src = %s\n", type->src->cmd);

	io_xprintf(io, "cname = %#s\n", type->cname);
	io_xprintf(io, "size = %d\n", type->size);
	io_xprintf(io, "recl = %d\n", type->recl);

	if	(type->dim)
		io_xprintf(io, "dim = %d\n", type->dim);

	io_putc('\n', io);

	if	(type->eval)	io_puts("eval()\n", io);

	if	(type->flags)
	{
		io_puts("flags:", io);

		if	(type->flags & TYPE_EXTERN)	io_puts("\textern", io);
		if	(type->flags & TYPE_ENUM)	io_puts("\tenum", io);
		if	(type->flags & TYPE_MALLOC)	io_puts("\tmalloc", io);

		io_putc('\n', io);
	}

	if	(type->base)
	{
		EfiType *t;

		io_puts("base:", io);

		for (t = type->base; t; t = t->base)
			io_xprintf(io, "\t%s", t->name);

		io_putc('\n', io);
	}

	io_puts("io:", io);

	if	(type->read)	io_puts("\tread", io);
	if	(type->write)	io_puts("\twrite", io);
	if	(type->print)	io_puts("\tprint", io);

	io_putc('\n', io);
	io_puts("copy:", io);

	if	(type->copy)
		io_puts("\tbuiltin", io);
	else if	(type->write && type->read)
		io_puts("\twrite/read", io);
	else	io_puts("\tmemcpy", io);

	if	(type->fcopy)
		io_puts("\tfunction", io);

	io_putc('\n', io);
	
	io_puts("clean:", io);

	if	(type->fclean)
		io_puts("\tfunction", io);

	if	(type->clean)
		io_puts("\tbuiltin", io);
	else	io_puts("\tmemset", io);

	io_putc('\n', io);
}

static void list_vfunc(IO *io, const VecBuf *tab)
{
	EfiFunc **ftab;
	int i;

	ftab = (EfiFunc **) tab->data;

	for (i = 0; i < tab->used; i++)
	{
		ListFunc(io, ftab[i]);
		io_puts("\n", io);
	}
}

static void tinfo_conv (IO *io, InfoNode *info)
{
	EfiType *type = info->par;
	list_vfunc(io, &type->conv);
}

static void tinfo_create (IO *io, InfoNode *info)
{
	EfiType *type = info->par;
	NameKeyEntry *p;
	size_t n, k;

	if	(type->create)
		list_vfunc(io, &type->create->tab);

	io_putc('\n', io);

	for (n = TypeTab.tab.used, p = TypeTab.tab.data; n-- > 0; p++)
	{
		EfiType *src = p->data;
		EfiFunc **ftab = src->conv.data;

		for (k = 0; k < src->conv.used; k++)
		{
			if (ftab[k]->type == type)
			{
				ListFunc(io, ftab[k]);
				io_puts("\n", io);
			}
		}
	}
}

static void tinfo_var (IO *io, InfoNode *info)
{
	EfiType *type = info->par;
	ShowVarTab(io, NULL, type->vtab);
}

static void print_par (IO *io, InfoNode *info)
{
	PrintEfiPar(io, info->par);
}

typedef struct {
	InfoNode *base;
	InfoNode *sub;
	const EfiPar *par;
} ADD;

static void add_par (EfiParClass *entry, void *data)
{
	ADD *add = data;

	if	(add->par != entry->epc_par)
	{
		add->par = entry->epc_par;
		add->sub = AddInfo(add->base,
			add->par->name, add->par->label, NULL, NULL);
	}

	AddInfo(add->sub, entry->epc_name, GetFormat(entry->epc_label),
		print_par, entry);
}

static void load_par (InfoNode *info)
{
	ADD add;
	add.base = info;
	add.sub = NULL;
	add.par = NULL;
	EfiParWalk(info->par, NULL, add_par, &add);
}

static void tinfo_par (IO *io, InfoNode *info)
{
	;
}

static void tinfo_list (IO *io, InfoNode *info)
{
	EfiType *type;
	EfiStruct *st;
	size_t size, loffset, tsize;

	type = info->par;
	tsize = 0;
	loffset = 0;

	for (st = type->list; st != NULL; st = st->next)
	{
		if	(tsize != st->offset && loffset != st->offset)
			io_xprintf(io, "@%d:%d\n", tsize, st->offset - tsize);

		size = st->type->size * (st->dim ? st->dim : 1);
		tsize = st->offset + size;
		loffset = st->offset;
		io_xprintf(io, "@%d:%d", st->offset, size);

		if	(st->member)
			io_putc('*', io);

		io_putc('\t', io);
		list_name(io, st);
		io_putc('\n', io);
	}
}

static void type_setup (InfoNode *info)
{
	EfiType *type = info->par;
	InfoNode *sub;

	AddInfo(info, "conv", ":en:Converters:de:Konverter",
		tinfo_conv, type);
	AddInfo(info, "create", ":en:Constructors:de:Konstruktoren",
		tinfo_create, type);
	AddInfo(info, "var", ":en:Variables:de:Variablen",
		tinfo_var, type);
	sub = AddInfo(info, "par", ":en:Parameters:de:Parameter",
		tinfo_par, type);
	sub->setup = load_par;

	if	(type->base)
	{
		InfoNode *base = AddInfo(info, "base",
			":en:base type:de:Basisdatentyp",
			info_type, type->base);
		base->setup = type_setup;
	}

	if	(type->list)
		AddInfo(info, "member", ":en:structure members"
			":de:Strukturkomponenten", tinfo_list, type);
}

static void entry_clean (void *data)
{
	rd_deref(*((InfoNode **) data));
}

static void tinfo_setup (InfoNode *info)
{
	static unsigned sync = 0;
	NameKeyEntry *p;
	size_t n;

	if	(sync != TypeTabChangeCount)
	{
		sync = TypeTabChangeCount;
		vb_clean(info->list, entry_clean);
	}
	else if	(info->list)
	{
		info->setup = tinfo_setup;
		return;
	}

	for (n = TypeTab.tab.used, p = TypeTab.tab.data; n-- > 0; p++)
	{
		EfiType *type = p->data;
		InfoNode *tnode = AddInfo(info, type->name, NULL,
			info_type, type);
		tnode->setup = type_setup;
	}

	info->setup = tinfo_setup;
}


static void f_type_info (EfiFunc *func, void *rval, void **arg)
{
	EfiType *type = Val_type(arg[0]);
	char *mode = Val_str(arg[1]);
	StrBuf *buf = sb_acquire();

	if	(mode)
	{
		sb_puts(mode, buf);
		sb_putc(':', buf);
	}

	sb_puts(KEY_TYPE, buf);

	if	(type && type->name)
	{
		sb_putc('/', buf);
		sb_puts(type->name, buf);
	}

	BrowseInfo(sb_nul(buf));
	sb_release(buf);
}

static void f_get_ctrl (EfiFunc *func, void *rval, void **arg)
{
	EfiControl *ctrl = SearchEfiPar(Val_type(arg[0]),
		&EfiPar_control, Val_str(arg[1]));
	Val_str(rval) = ctrl ? mstrcpy(ctrl->data) : NULL;
}

static void f_list_ctrl (EfiFunc *func, void *rval, void **arg)
{
	EfiType *type = Val_type(arg[0]);
	IO *io = Val_io(arg[1]);
	ListEfiPar(io, type, &EfiPar_control, NULL, 1);
}

static void f_set_ctrl (EfiFunc *func, void *rval, void **arg)
{
	EfiType *type = Val_type(arg[0]);
	char *name = Val_str(arg[1]);
	char *def = Val_str(arg[2]);
	EfiControl *ctrl = GetEfiPar(type, &EfiPar_control, name);

	if	(ctrl)
	{
		memfree(ctrl->data);
		ctrl->data = mstrcpy(def);
	}
	else
	{
		ctrl = memalloc(sizeof *ctrl);
		ctrl->epc_par = &EfiPar_control;
		ctrl->epc_type = Val_bool(arg[3]) ? NULL : type;
		ctrl->epc_name = mstrcpy(name);
		ctrl->epc_label = mstrcpy(Val_str(arg[4]));
		ctrl->epc_info = EfiControl_info;
		ctrl->data = mstrcpy(def);
		AddEfiPar(type, ctrl);
	}
}

static void g_set_ctrl (EfiFunc *func, void *rval, void **arg)
{
	char *name = Val_str(arg[0]);
	char *def = Val_str(arg[1]);
	EfiControl *ctrl = GetEfiPar(NULL, &EfiPar_control, name);

	if	(ctrl)
	{
		memfree(ctrl->data);
		ctrl->data = mstrcpy(def);
	}
	else
	{
		ctrl = memalloc(sizeof *ctrl);
		ctrl->epc_par = &EfiPar_control;
		ctrl->epc_type = NULL;
		ctrl->epc_name = mstrcpy(name);
		ctrl->epc_label = mstrcpy(Val_str(arg[2]));
		ctrl->epc_info = EfiControl_info;
		ctrl->data = mstrcpy(def);
		AddEfiPar(NULL, ctrl);
	}
}

static void f_is_type (EfiFunc *func, void *rval, void **arg)
{
	EfiType *type = GetType(Val_str(arg[0]));
	Val_bool(rval) = type ? 1 : 0;
}

static void f_make_class (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = MakeEfiClass(Val_type(arg[0]),
		Val_str(arg[1]), Val_str(arg[2]));
}


static EfiFuncDef fdef_type[] = {
	{ 0, &Type_void, "Type_t::info (str mode = NULL)", f_type_info },
	{ 0, &Type_str, "Type_t::get_ctrl (str name)", f_get_ctrl },
	{ 0, &Type_void, "Type_t::set_ctrl (str name, str value,"
		"bool inheritable = false, str label = NULL)", f_set_ctrl },
	{ 0, &Type_void, "set_ctrl (str name, str value, str label = NULL)",
		g_set_ctrl },
	{ 0, &Type_void, "Type_t::list_ctrl (IO out = iostd)", f_list_ctrl },
	{ 0, &Type_bool, "is_type (str name)", f_is_type },
	{ 0, &Type_bool, "Type_t::classification (str name, str def)",
		f_make_class },

};

/*	Initialisierung
*/

void CmdSetup_type (void)
{
	InfoNode *info;

	AddEfiMember(Type_type.vtab, type_member, tabsize(type_member));
	AddFuncDef(fdef_type, tabsize(fdef_type));
	info = AddInfo(NULL, KEY_TYPE, LBL_TYPE, NULL, NULL);
	info->setup = tinfo_setup;
}
