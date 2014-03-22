/*
Operatoren definieren

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
#include <EFEU/cmdconfig.h>

#define	STR(n)	Val_str(arg[n])


static void MakeGlobalFunc(char *p)
{
	VarTab_xadd(GlobalVar, p, NULL,
		NewPtrObj(&Type_vfunc, VirFunc(NULL)));
}

static void f_prior (EfiFunc *func, void *rval, void **arg)
{
	register EfiOp *op = nkt_fetch(&PostfixTab, STR(0), NULL);
	Val_int(rval) = op ? op->prior : 0;
}

static void f_leftop (EfiFunc *func, void *rval, void **arg)
{
	EfiOp *op;

	op = memalloc(sizeof(EfiOp));
	op->name = mstrcpy(STR(0));
	op->prior = OpPrior_Unary;
	op->assoc = OpAssoc_Left;
	op->parse = PrefixOp;
	AddOpDef(&PrefixTab, op, 1);
	MakeGlobalFunc(mstrcat(NULL, op->name, "()", NULL));
}

static void f_rightop (EfiFunc *func, void *rval, void **arg)
{
	EfiOp *op;

	op = memalloc(sizeof(EfiOp));
	op->name = mstrcpy(STR(0));
	op->prior = OpPrior_Unary;
	op->assoc = OpAssoc_Left;
	op->parse = PostfixOp;
	AddOpDef(&PostfixTab, op, 1);
	MakeGlobalFunc(mstrcpy(op->name));
}

static void f_binop (EfiFunc *func, void *rval, void **arg)
{
	EfiOp *op;

	op = memalloc(sizeof(EfiOp));
	op->name = mstrcpy(STR(0));
	op->prior = Val_int(arg[1]);
	op->assoc = Val_bool(arg[2]) ? OpAssoc_Left : OpAssoc_Right;
	op->parse = BinaryOp;
	AddOpDef(&PostfixTab, op, 1);
	MakeGlobalFunc(mstrcpy(op->name));
}

static EfiFuncDef fdef_op[] = {
	{ 0, &Type_int, "Prior (str op)", f_prior },
	{ 0, &Type_void, "RightOp (str op)", f_rightop },
	{ 0, &Type_void, "LeftOp (str op)", f_leftop },
	{ 0, &Type_void, "BinOp (str op, int prior, bool assoc = false)",
		f_binop },
};


void CmdSetup_op(void)
{
	AddFuncDef(fdef_op, tabsize(fdef_op));
}
