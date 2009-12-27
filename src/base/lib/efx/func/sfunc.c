/*
Funktion mit minimaler Distanz suchen

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
#include <EFEU/konvobj.h>
#include <EFEU/Debug.h>

int FuncDebugLock = 0;

static io_t *FuncDebugLog = NULL;
static int FuncDebugSync = 0;

static int debug_mode (void)
{
	if	(FuncDebugLock)	return 0;

	if	(FuncDebugSync < DebugChangeCount)
	{
		FuncDebugSync = DebugChangeCount;
		rd_deref(FuncDebugLog);
		FuncDebugLog = rd_refer(LogOut("func", DBG_DEBUG));
	}

	return (FuncDebugLog != NULL);
}

void FuncDebug (Func_t *func, const char *pfx)
{
	if	(func && debug_mode())
	{
		if	(pfx)
		{
			io_puts(pfx, FuncDebugLog);
			io_putc('\t', FuncDebugLog);
		}

		ListFunc(FuncDebugLog, func);
		io_putc('\n', FuncDebugLog);
	}
}

static int test_base(const Type_t *old, const Type_t *new)
{
	for (; old != NULL; old = old->dim ? NULL : old->base)
		if (old == new) return 1;

	return 0;
}


static void show_dist(io_t *io, Func_t *func, int dist,
	ArgKonv_t *konv, size_t dim)
{
	int i;

	FuncDebugLock++;
	io_printf(io, "%#.2x\t", dist);
	ListFunc(io, func);
	io_putc('\n', io);

	for (i = 0; i < dim; i++)
	{
		io_printf(io, "%d:\t%#.2x\t%s\t", i, konv[i].dist,
			konv[i].type ? konv[i].type->name : ".");
		ListFunc(io, konv[i].func);
		io_putc('\n', io);
	}

	FuncDebugLock--;
}

static int get_konvdef(Func_t *func, size_t narg,
	FuncArg_t *arg, ArgKonv_t *konv);

Func_t *SearchFunc(VirFunc_t *vtab, FuncArg_t *arg,
	size_t narg, ArgKonv_t **ptr)
{
	ArgKonv_t *x, *konv, *curkonv;
	int i, dist, curdist;
	Func_t *func, *curfunc, **ftab;

	if	(vtab == NULL || vtab->tab.used == 0)
		return NULL;

	if	(vtab->reftype != &VirFuncRefType)
	{
		liberror(MSG_EFMAIN, 65);
		return NULL;
	}

/*	Funktion suchen
*/
	if	(narg != 0)
	{
		konv = ALLOC(narg, ArgKonv_t);
		curkonv = ALLOC(narg, ArgKonv_t);
	}
	else	konv = curkonv = NULL;

	dist = D_MAXDIST;
	ftab = vtab->tab.data;
	func = NULL;

	for (i = 0; i < vtab->tab.used; i++)
	{
		curfunc = ftab[i];
		curdist = get_konvdef(curfunc, narg, arg, curkonv);

		if	(func && debug_mode())
			show_dist(iostd, curfunc, curdist, curkonv, narg);

		switch (curdist)
		{
		case D_REJECT:
			continue;
		case D_MATCH:
			memfree(konv);
			memfree(curkonv);

			if	(ptr)	*ptr = NULL;

			return curfunc;
		default:
			break;
		}

		if	(dist > curdist)
		{
			x = konv;
			konv = curkonv;
			curkonv = x;
			dist = curdist;
			func = curfunc;
		}
	}

	memfree(curkonv);

	if	(func && ptr)
	{
		*ptr = konv;
	}
	else	memfree(konv);

	return func;
}


static int get_konvdef(Func_t *func, size_t narg,
	FuncArg_t *arg, ArgKonv_t *konv)
{
	int i, dist;

	if	(narg < func->dim - func->vaarg && !func->arg[narg].defval)
		return D_REJECT;

	if	(narg > func->dim && !func->vaarg)
		return D_REJECT;

	dist = 0;

	for (i = dist = 0; i < narg; i++)
	{
		ArgKonv_t *x = konv + i;
		konv[i].func = NULL;
		konv[i].type = NULL;
		konv[i].dist = 0;

		if	(i >= (func->dim - func->vaarg))
			konv[i].dist = D_VAARG;
		else if	(func->arg[i].lval && !arg[i].lval)
			return D_REJECT;
		else if	(func->arg[i].type == arg[i].type)
			konv[i].dist = D_MATCH;
		else if	(func->arg[i].type == NULL)
			konv[i].dist = D_ACCEPT;
		/*
		else if	(func->arg[i].lval || func->arg[i].nokonv)
			return D_REJECT;
		*/
		else if	(test_base(arg[i].type, func->arg[i].type))
			konv[i].dist = D_BASE;
		else if	(func->arg[i].lval || func->arg[i].nokonv)
			return D_REJECT;
		else if	((x = GetArgKonv(arg[i].type, func->arg[i].type)))
			konv[i] = *x;
		else	return D_REJECT;

		if	(dist < konv[i].dist)	dist = konv[i].dist;
	}

	return dist;
}
