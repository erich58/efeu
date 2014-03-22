/*	Aggregationsstruktur

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

#include <EFEU/EDB.h>
#include <EFEU/konvobj.h>

#define	ERR_AGGR "[edb:aggr]$!: aggregate function $1 " \
	"is not definied for data type $2.\n"

static void aggr_clean (void *ptr)
{
	rd_deref(((EDBAggregate *) ptr)->next);
	rd_deref(((EDBAggregate *) ptr)->sub);
	rd_deref(((EDBAggregate *) ptr)->var);
	memfree(ptr);
}

static RefType aggr_reftype = REFTYPE_INIT("EDBAggregate", NULL, aggr_clean);

EDBAggregate *NewEDBAggregate (EDBAggregate *next, const char *op,
	EfiStruct *var, unsigned offset)
{
	EDBAggregate *x = memalloc(sizeof *x);
	x->next = next;
	x->sub = NULL;
	x->var = rd_refer(var);
	x->offset = offset;
	x->f_aggregate = GetFunc(NULL, GetTypeFunc(var->type, "+="),
		2, var->type, 1, var->type, 0); 

	if	(!x->f_aggregate)
		log_note(NULL, ERR_AGGR, "ss", op, var->type->name);

	return rd_init(&aggr_reftype, x);
}

void EDBAggregateData (EDBAggregate *aggr, void *opaque_tg, void *opaque_src)
{
	for (; aggr != NULL; aggr = aggr->next)
	{
		void *arg[2];
		size_t n;

		arg[0] = (char *) opaque_tg + aggr->var->offset;
		arg[1] = (char *) opaque_src + aggr->offset;

		if	(aggr->sub)
		{
			EDBAggregateData(aggr->sub, arg[0], arg[1]);
			continue;
		}
		else if	(!aggr->f_aggregate)
		{
			continue;
		}

		CallVoidFuncVec(aggr->f_aggregate, arg);

		for (n = 1; n < aggr->var->dim; n++)
		{
			arg[0] = ((char *) arg[0]) + aggr->var->type->size;
			arg[1] = ((char *) arg[1]) + aggr->var->type->size;
			CallVoidFuncVec(aggr->f_aggregate, arg);
		}
	}
}

static void func_aggr (EfiFunc *func, void *rval, void **arg)
{
	EDBAggregateData(func->par, arg[0], arg[1]);
}

void EDBAggregateFunc (EDBAggregate *aggr, const char *name,
	EfiType *tg, EfiType *base)
{
	EfiFunc *func;
	IO *io;

	if	(!aggr)	return;

	if	(base == tg || GetKonv(NULL, base, tg))
	{
		rd_deref(aggr);
		return;
	}

	io = io_tmpbuf(0);
	io_puts(tg->name, io);
	io_xprintf(io, "::operator%#s (", name);
	io_puts(base->name, io);
	io_puts(")", io);
	io_rewind(io);
	func = MakePrototype(io, tg, NULL, FUNC_LRETVAL|FUNC_VIRTUAL);
	func->par = aggr;
	func->eval = func_aggr;
	func->clean = rd_clean;
	AddFunc(func);
	io_close(io);
}
