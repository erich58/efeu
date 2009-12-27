/*	Operatoren definieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/Op.h>
#include <EFEU/cmdconfig.h>

#define	STR(n)	Val_str(arg[n])


static void MakeGlobalFunc(const char *p)
{
	Var_t *var = NewVar(&Type_vfunc, p, 0);
	Val_vfunc(var->data) = VirFunc(NULL);
	AddVar(GlobalVar, var, 1);
}

static void f_prior (Func_t *func, void *rval, void **arg)
{
	register Op_t *op = skey_find(&PostfixTab, STR(0));
	Val_int(rval) = op ? op->prior : 0;
}

static void f_leftop (Func_t *func, void *rval, void **arg)
{
	Op_t *op;
	char *p;

	op = ALLOC(1, Op_t);
	op->name = mstrcpy(STR(0));
	op->prior = OpPrior_Unary;
	op->assoc = OpAssoc_Left;
	op->parse = PrefixOp;
	xsearch(&PrefixTab, op, XS_REPLACE);
	p = mstrcat(NULL, op->name, "()", NULL);
	MakeGlobalFunc(p);
	FREE(p);
}

static void f_rightop (Func_t *func, void *rval, void **arg)
{
	Op_t *op;

	op = ALLOC(1, Op_t);
	op->name = mstrcpy(STR(0));
	op->prior = OpPrior_Unary;
	op->assoc = OpAssoc_Left;
	op->parse = PostfixOp;
	xsearch(&PostfixTab, op, XS_REPLACE);
	MakeGlobalFunc(op->name);
}

static void f_binop (Func_t *func, void *rval, void **arg)
{
	Op_t *op;

	op = ALLOC(1, Op_t);
	op->name = mstrcpy(STR(0));
	op->prior = Val_int(arg[1]);
	op->assoc = Val_bool(arg[2]) ? OpAssoc_Left : OpAssoc_Right;
	op->parse = BinaryOp;
	xsearch(&PostfixTab, op, XS_REPLACE);
	MakeGlobalFunc(op->name);
}

static FuncDef_t fdef_op[] = {
	{ 0, &Type_int, "Prior (str op)", f_prior },
	{ 0, &Type_void, "RightOp (str op)", f_rightop },
	{ 0, &Type_void, "LeftOp (str op)", f_leftop },
	{ 0, &Type_void, "BinOp (str op, long prior, bool assoc = false)",
		f_binop },
};


void CmdSetup_op(void)
{
	AddFuncDef(fdef_op, tabsize(fdef_op));
}
