/*	Funktionsdefinition anhängen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>


void SetFunc(unsigned flags, Type_t *type, const char *fmt, FuncEval_t eval)
{
	Func_t *func = Prototype(fmt, type, NULL, flags);

	if	(func)
	{
		func->eval = eval;
		AddFunc(func);
	}
}

void AddFuncDef(FuncDef_t *def, size_t dim)
{
	for (; dim-- != 0; def++)
		SetFunc(def->flags, def->type, def->prot, def->eval);
}


void DelFuncArg(FuncArg_t *arg, size_t dim)
{
	if	(arg)
	{
		while (dim-- != 0)
		{
			memfree(arg[dim].name);
			UnrefObj(arg[dim].defval);
		}

		memfree(arg);
	}
}
