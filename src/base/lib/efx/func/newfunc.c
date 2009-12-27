/*	Funktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/refdata.h>

static ALLOCTAB(tab_func, 0, sizeof(Func_t));

static Func_t *func_admin(Func_t *tg, const Func_t *src)
{
	if	(tg)
	{
		DelFuncArg(tg->arg, tg->dim);
		memfree(tg->name);

		if	(tg->clean)
			tg->clean(tg->par);

		del_data(&tab_func, tg);
		return NULL;
	}
	else	return new_data(&tab_func);
}

static char *func_ident(Func_t *func)
{
	char *p;

	if	(func->name)	p = func->name;
	else if	(func->type)	p = func->type->name;
	else			p = Type_obj.name;

	return msprintf("%s()", p);
}

REFTYPE(FuncRefType, "Func", func_ident, func_admin);


Func_t *NewFunc(void)
{
	return rd_create(&FuncRefType);
}
