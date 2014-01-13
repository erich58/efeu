/*
Daten über Ausdruck generieren
*/

#include <EFEU/EDB.h>
#include <EFEU/ioctrl.h>
#include <EFEU/printobj.h>
#include <EFEU/cmdeval.h>
#include <EFEU/DBData.h>
#include <ctype.h>

typedef struct {
	REFVAR;
	EfiObj *obj;
	EfiObj *expr;
} EXPR;

static void expr_clean (void *data)
{
	EXPR *par = data;
	UnrefObj(par->obj);
	UnrefObj(par->expr);
	memfree(par);
}

static RefType expr_reftype = REFTYPE_INIT("EDB_EXPR", NULL, expr_clean);

static int read_expr (EfiType *type, void *data, void *opaque_par)
{
	EXPR *par = opaque_par;
	int stat = 1;

	PushVarTab(RefVarTab(par->obj->type->vtab), RefObj(par->obj));

start:
	CmdEval_stat = 0;
	UnrefEval(RefObj(par->expr));

	switch (CmdEval_stat)
	{
	case CmdEval_Break:
	case CmdEval_Return:
		stat = 0;
		break;
	case CmdEval_Cont:
		goto start;
	default:
		stat = 1;
		break;
	}

	PopVarTab();
	CmdEval_stat = 0;
	return stat;
}

void edb_expr (EDB *edb, EfiObj *expr)
{
	EXPR *par;

	if	(!edb || !expr)
	{
		UnrefObj(expr);
		return;
	}

	par = memalloc(sizeof *par);
	par->obj = RefObj(edb->obj);
	par->expr = expr;
	rd_init(&expr_reftype, par);

	edb->save = 0;
	edb->read = read_expr;
	edb->ipar = par;
}
