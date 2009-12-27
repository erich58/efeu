/*
Abfrage eines Operators

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
#include <EFEU/Op.h>


static char parsebuf[IO_MAX_SAVE];
static int parsepos = 0;
static int prior = 0;
static Op_t *opdef = NULL;
static io_t *io = NULL;

static int doparse (Op_t *op);
static void pushback (int save);


static int doparse(Op_t *op)
{
	int i;

	for (i = 0; op->name[i] != 0; i++)
	{
		if	(i >= IO_MAX_SAVE)
		{
			return 1;	/* Weitersuchen */
		}

		if	(i >= parsepos)
		{
			int c;

			if	((c = io_mgetc(io, 1)) == EOF)
			{
				parsebuf[parsepos++] = 0;
			}
			else	parsebuf[parsepos++] = c;
		}

		if	(op->name[i] != parsebuf[i])
		{
			return 1;	/* Weitersuchen */
		}
	}

/*	Gefunden: Test auf korrekte Priorität
*/
	if	(op->prior < prior)
	{
		return 0;
	}
	else if	(op->prior == prior && op->assoc == OpAssoc_Left)
	{
		return 0;
	}

/*	Zuviel gelesene Zeichen zurückschreiben
*/
	opdef = op;
	pushback(i);
	return 0;
}


Obj_t *Parse_op(io_t *in, int p, Obj_t *obj)
{
	if	(io_eat(in, " \t") == EOF)
	{
		return NULL;
	}

	io = in;
	parsepos = 0;
	opdef = NULL;
	prior = p;
	xwalk(obj ? &PostfixTab : &PrefixTab, (visit_t) doparse);

	if	(opdef == NULL)
	{
		pushback(0);
		return NULL;
	}

	obj = opdef->parse(in, opdef, obj);
	return (obj ? obj : Obj_noop());
}


/*	Zeichen zurückschreiben
*/

static void pushback(int save)
{
	while (parsepos > save)
	{
		parsepos--;

		if	(parsebuf[parsepos] != 0)
		{
			io_ungetc(parsebuf[parsepos], io);
		}
	}
}


/*	Parse - Funktionen
*/

Obj_t *UnaryTerm(const char *a, Obj_t *obj, int flag)
{
	ObjList_t *list;
	char *p;
	void *func;

	if	(obj == NULL)	return NULL;

	p = mstrcat(NULL, a, flag ? NULL : "()", NULL);
	func = GetGlobalFunc(p);
	memfree(p);

	if	(func == NULL)	return obj;

	list = NewObjList(obj);

	if	(!obj->type->eval)
	{
		obj = EvalFunc(func, list);
		DelObjList(list);
		return obj;
	}

	return Obj_call(Expr_func, func, list);
}


Obj_t *PrefixOp(io_t *io, Op_t *op, Obj_t *left)
{
	if	((left = Parse_term(io, op->prior)) == NULL)
	{
		io_error(io, MSG_EFMAIN, 101, 1, op->name);
		return NULL;
	}

	return UnaryTerm(op->name, left, 0);
}


Obj_t *PostfixOp(io_t *io, Op_t *op, Obj_t *left)
{
	return UnaryTerm(op->name, left, 1);
}


Obj_t *BinaryOp(io_t *io, Op_t *op, Obj_t *left)
{
	Obj_t *right;

	right = Parse_term(io, op->prior);

	if	(right == NULL)
	{
		io_error(io, MSG_EFMAIN, 101, 1, op->name);
		return left;
	}

	return BinaryTerm(op->name, left, right);
}


Obj_t *BinaryTerm(const char *name, Obj_t *left, Obj_t *right)
{
	ObjList_t *list;
	void *func;

	func = GetGlobalFunc(name);

	if	(func == NULL || left == NULL || right == NULL)
	{
		if	(func == NULL)
		{
			reg_cpy(1, name);
			errmsg(MSG_EFMAIN, 70);
		}

		UnrefObj(left);
		UnrefObj(right);
		return NULL;
	}

	list = NewObjList(left);
	list->next = NewObjList(right);

	if	(!left->type->eval && !right->type->eval)
	{
		left = EvalFunc(func, list);
		DelObjList(list);
		return left;
	}

	return Obj_call(Expr_func, func, list);
}
