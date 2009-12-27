/*
Programmsteuerung

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
#include <EFEU/cmdsetup.h>
#include <EFEU/parsedef.h>
#include <EFEU/konvobj.h>
#include <EFEU/locale.h>


static void list_vtab(IO *io, EfiVarTab *tab, int limit)
{
	if	(tab == NULL || tab->tab.used <= limit)
		ShowVarTab(io, NULL, tab);
	else if	(tab->name)
		io_printf(io, "%s[%d]", tab->name, tab->tab.used);
	else	io_printf(io, "%#p[%d]", tab, tab->tab.used);
}

static void f_vtabstack (EfiFunc *func, void *rval, void **arg)
{
	EfiVarStack *stack; 
	IO *io;
	int limit;

	limit = Val_int(arg[0]);
	io = Val_io(arg[1]);
	list_vtab(io, LocalVar, limit);

	for (stack = VarStack; stack != NULL; stack = stack->next)
	{
		io_puts(", ", io);
		list_vtab(io, stack->tab, limit);

		if	(stack->obj)
			io_printf(io, "(%s)", stack->obj->type->name);

	}

	io_putc('\n', io);
}


static void f_typedist (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = KonvDist(Val_type(arg[0]), Val_type(arg[1]));
}


static struct {
	int flag;
	char *name;
} dist_tab[] = {
	{ D_MAXDIST, "MAXDIST" },
	{ D_CREATE, "CREATE" },
	{ D_RESTRICTED, "RESTRICTED" },
	{ D_VAARG, "VAARG" },
	{ D_ACCEPT, "ACCEPT" },
	{ D_BASE, "BASE" },
	{ D_KONVERT, "KONVERT" },
	{ D_EXPAND, "EXPAND" },
	{ D_PROMOTE, "PROMOTE" },
};

static void f_showkonv (EfiFunc *func, void *rval, void **arg)
{
	EfiType *old, *new;
	EfiKonv konv;

	old = Val_type(arg[0]);
	new = Val_type(arg[1]);

	if	(old == NULL || new == NULL)
		return;

	io_printf(iostd, "%s -> %s: ", old->name, new->name);

	if	(GetKonv(&konv, old, new))
	{
		char *delim;
		int i;

		io_printf(iostd, "%#4o", konv.dist);
		delim = " ";

		for (i = 0; i < tabsize(dist_tab); i++)
		{
			if	(konv.dist & dist_tab[i].flag)
			{
				io_puts(delim, iostd);
				io_puts(dist_tab[i].name, iostd);
				delim = "|";
			}
		}

		io_putc('\n', iostd);
	}
	else	io_puts("-1 REJECT\n", iostd);
}

static void f_lcshow (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);

	if	(Locale.scan)
	{
		io_printf(io, "scan.thousands_sep = %#s\n",
			Locale.scan->thousands_sep);
		io_printf(io, "scan.decimal_point = %#s\n",
			Locale.scan->decimal_point);
	}

	if	(Locale.print)
	{
		io_printf(io, "print.thousands_sep = %#s\n",
			Locale.print->thousands_sep);
		io_printf(io, "print.decimal_point = %#s\n",
			Locale.print->decimal_point);
		io_printf(io, "print.negativ_sign = %#s\n",
			Locale.print->negative_sign);
		io_printf(io, "print.positive_sign = %#s\n",
			Locale.print->positive_sign);
		io_printf(io, "print.zero_sign = %#s\n",
			Locale.print->zero_sign);
	}
}

static void f_locale (EfiFunc *func, void *rval, void **arg)
{
	IO *io = Val_io(arg[0]);

	if	(Locale.scan)
		io_printf(io, "LC_SCAN=%#s\n", Locale.scan->name);

	if	(Locale.print)
		io_printf(io, "LC_PRINT=%#s\n", Locale.print->name);

	if	(Locale.date)
		io_printf(io, "LC_DATE=%#s\n", Locale.date->name);
}

static void f_parselist (EfiFunc *func, void *rval, void **arg)
{
	ListParseDef(Val_io(arg[0]));
}

static void f_typehead (EfiFunc *func, void *rval, void **arg)
{
	Val_str(rval) = TypeHead(Val_type(arg[0]));
}

static void f_getconv (EfiFunc *func, void *rval, void **arg)
{
	EfiKonv *konv = GetKonv(NULL, Val_type(arg[0]), Val_type(arg[1]));
	Val_func(rval) = konv ? rd_refer(konv->func) : NULL;
}

static EfiObj *func_type (const EfiObj *base, void *data)
{
	EfiFunc *func = base ? Val_ptr(base->data) : NULL;
	return func ? ConstObj(&Type_type, &func->type) : NULL;
}

static EfiMember var_func[] = {
	{ "type", &Type_type, func_type, NULL },
};

/*	Initialisierung
*/

void CmdSetup_test(void)
{
	SetFunc(0, &Type_void, "vtabstack (int = 0, IO = iostd)", f_vtabstack);
	SetFunc(0, &Type_int, "dist (Type_t a, Type_t b)", f_typedist);
	SetFunc(0, &Type_void, "showkonv (Type_t a, Type_t b)", f_showkonv);
	SetFunc(0, &Type_func, "getconv (Type_t a, Type_t b)", f_getconv);
	SetFunc(0, &Type_func, "Type_t::getconv (Type_t b)", f_getconv);
	SetFunc(0, &Type_void, "lcshow (IO = iostd)", f_lcshow);
	SetFunc(0, &Type_void, "locale (IO = iostd)", f_locale);
	SetFunc(0, &Type_void, "parselist (IO = iostd)", f_parselist);
	SetFunc(0, &Type_str, "typehead (Type_t type)", f_typehead);
	AddEfiMember(Type_func.vtab, var_func, tabsize(var_func));
}
