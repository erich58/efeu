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
#include <EFEU/conv.h>
#include <EFEU/Debug.h>

int FuncDebugLock = 0;

static LogControl func_debug = LOG_CONTROL("func", LOGLEVEL_DEBUG);
static LogControl func_trace = LOG_CONTROL("func", LOGLEVEL_TRACE);

void FuncDebug (EfiFunc *func, const char *pfx)
{
	IO *out;

	if	(FuncDebugLock)	return;

	if	(func && (out = LogOpen(&func_debug)))
	{
		if	(pfx)
		{
			io_puts(pfx, out);
			io_putc('\t', out);
		}

		ListFunc(out, func);
		io_putc('\n', out);
		io_close(out);
	}
}

static int test_base(const EfiType *src, const EfiType *tg)
{
	for (; src != NULL; src = src->dim ? NULL : src->base)
		if (src == tg) return 1;

	return 0;
}


static void show_dist(IO *io, EfiFunc *func, int dist,
	EfiConv *conv, size_t dim)
{
	int i;

	FuncDebugLock++;
	io_xprintf(io, "%#.4x  ", dist & 0xffff);
	ListFunc(io, func);
	io_putc('\n', io);

	for (i = 0; i < dim; i++)
	{
		io_xprintf(io, "%5d:  %#.4x\t%s\t", i, conv[i].dist & 0xffff,
			conv[i].type ? conv[i].type->name : ".");
		ListFunc(io, conv[i].func);
		io_putc('\n', io);
	}

	FuncDebugLock--;
}

static int get_convdef(EfiFunc *func, size_t narg,
	EfiFuncArg *arg, EfiConv *conv);

EfiFunc *SearchFunc (EfiVirFunc *vtab, EfiFuncArg *arg,
	size_t narg, EfiConv **ptr)
{
	EfiConv *x, *konv, *curkonv;
	int i, dist, curdist;
	EfiFunc *func, *curfunc, **ftab;
	IO *out;

	if	(vtab == NULL || vtab->tab.used == 0)
		return NULL;

	if	(ptr)	*ptr = NULL;

	if	(!IsVirFunc(vtab))
	{
		log_error(NULL, "[efmain:65]", NULL);
		return NULL;
	}

/*	Funktion suchen
*/
	if	(narg != 0)
	{
		konv = memalloc(narg * sizeof(EfiConv));
		curkonv = memalloc(narg * sizeof(EfiConv));
	}
	else	konv = curkonv = NULL;

	dist = D_MAXDIST;
	ftab = vtab->tab.data;
	func = NULL;

	for (i = 0; i < vtab->tab.used; i++)
	{
		curfunc = ftab[i];
		curdist = get_convdef(curfunc, narg, arg, curkonv);

		if	((out = LogOpen(&func_trace)))
		{
			show_dist(out, curfunc, curdist, curkonv, narg);
			io_close(out);
		}

		switch (curdist)
		{
		case D_REJECT:
			continue;
		case D_MATCH:
			if	(ptr && narg != curfunc->dim)
			{
				*ptr = curkonv;
			}
			else	memfree(curkonv);

			memfree(konv);
			return curfunc;
		default:
			break;
		}

		if	(curdist >= D_RESTRICTED)
			continue;

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


static int get_convdef(EfiFunc *func, size_t narg,
	EfiFuncArg *arg, EfiConv *konv)
{
	int i, dist;

	if	(narg < func->dim - func->vaarg && !func->arg[narg].defval)
		return D_REJECT;

	if	(narg > func->dim && !func->vaarg)
		return D_REJECT;

	dist = 0;

	for (i = dist = 0; i < narg; i++)
	{
		EfiConv *x = konv + i;
		konv[i].func = NULL;
		konv[i].type = NULL;
		konv[i].dist = 0;

		if	(i >= (func->dim - func->vaarg))
			konv[i].dist = D_VAARG;
		else if	(func->arg[i].lval && !arg[i].lval)
			return konv[i].dist = D_REJECT;
		else if	(func->arg[i].type == arg[i].type)
			konv[i].dist = D_MATCH;
		else if	(func->arg[i].type == NULL)
			konv[i].dist = D_ACCEPT;
		else if	(func->arg[i].noconv)
			return konv[i].dist = D_REJECT;
		else if	(func->arg[i].lval)
		{
			if	(test_base(arg[i].type, func->arg[i].type))
				konv[i].dist = D_BASE;
			else	return konv[i].dist = D_REJECT;
		}
		else if	((x = GetArgConv(arg[i].type, func->arg[i].type)))
		{
			if	((x->dist & D_RESTRICTED) &&
					(func->arg[i].promote ||
					func->weight == CONV_PROMOTION))
				x->dist = D_CONVERT | D_PROMOTE;

			konv[i] = *x;
		}
		else if	(test_base(arg[i].type, func->arg[i].type))
		{
			konv[i].dist = D_BASE;
		}
		else	return konv[i].dist = D_REJECT;

		if	(dist < konv[i].dist)	dist = konv[i].dist;
	}

	return dist;
}
