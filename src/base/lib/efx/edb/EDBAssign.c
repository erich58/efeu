/*	Zuweisungsstruktur

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

#include <EFEU/EDB.h>
#include <EFEU/konvobj.h>

static void assign_clean (void *ptr)
{
	rd_deref(((EDBAssign *) ptr)->next);
	rd_deref(((EDBAssign *) ptr)->sub);
	rd_deref(((EDBAssign *) ptr)->var);
	memfree(ptr);
}

static RefType assign_reftype = REFTYPE_INIT("EDBAssign", NULL, assign_clean);

EDBAssign *NewEDBAssign (EDBAssign *next, EfiObj *base, EfiStruct *st,
	EfiStruct *var, unsigned offset)
{
	EDBAssign *x = memalloc(sizeof *x);
	x->next = next;
	x->sub = NULL;
	x->var = rd_refer(var);
	x->offset = offset;
	x->base = RefObj(base);
	x->st = rd_refer(st);
	return rd_init(&assign_reftype, x);
}

void EDBAssignData (EDBAssign *assign, void *opaque_tg, void *opaque_src)
{
	for (; assign != NULL; assign = assign->next)
	{
		char *tg = (char *) opaque_tg + assign->var->offset;
		char *src = (char *) opaque_src + assign->offset;
		size_t n = assign->var->dim ? assign->var->dim : 1;

		if	(assign->sub)
		{
			EDBAssignData(assign->sub, tg, src);
		}
		else if	(assign->base)
		{
			assign->base->data = src;
			Obj2Data(assign->st->member(assign->st, assign->base),
				assign->var->type, tg);
		}
		else	AssignVecData(assign->var->type, n, tg, src);
	}
}

static void func_assign (EfiFunc *func, void *rval, void **arg)
{
	EDBAssignData(func->par, arg[0], arg[1]);
}

static void func_conv (EfiFunc *func, void *rval, void **arg)
{
	EDBAssignData(((EfiFunc *) func->par)->par, rval, arg[0]);
}

void EDBAssignFunc (EDBAssign *assign, EfiType *tg, EfiType *base)
{
	EfiFunc *func;
	EfiFunc *conv;
	IO *io;

	if	(!assign)	return;

	if	(base == tg || GetKonv(NULL, base, tg))
	{
		rd_deref(assign);
		return;
	}

	io = io_tmpbuf(0);
	io_puts(tg->name, io);
	io_puts("::operator:= (", io);
	io_puts(base->name, io);
	io_puts(")", io);
	io_rewind(io);
	func = MakePrototype(io, tg, NULL, FUNC_LRETVAL|FUNC_VIRTUAL);
	func->par = assign;
	func->eval = func_assign;
	func->clean = rd_deref;
	AddFunc(func);

	io_rewind(io);
	io_puts(base->name, io);
	io_puts("()", io);
	io_rewind(io);
	conv = MakePrototype(io, tg, NULL, 0);

	conv->par = rd_refer(func);
	conv->eval = func_conv;
	conv->clean = rd_deref;
	AddFunc(conv);
	io_close(io);
}

static int print_depth = 0;

void PrintEDBAssign (IO *out, EDBAssign *assign)
{
	if	(!out)	return;
	
	for (; assign; assign = assign->next)
	{
		io_nputc('\t', out, print_depth);
		io_printf(out, "%d %s %s",
			assign->var->offset,
			assign->var->type->name,
			assign->var->name);

		io_printf(out, " <- %d", assign->offset);

		if	(assign->base)
			io_printf(out, " %s %s::%s",
				assign->st->type->name,
				assign->base->type->name,
				assign->st->name);

		io_putc('\n', out);

		if	(assign->sub)
		{
			print_depth++;
			PrintEDBAssign(out, assign->sub);
			print_depth--;
		}
	}
}
