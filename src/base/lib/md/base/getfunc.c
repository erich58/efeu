/*	Funktionen abfragen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mdmat.h>

Func_t *mdfunc_add (const Type_t *type)
{
	Func_t *func;

	func = GetFunc((Type_t *) type, GetTypeFunc(type, "+="),
		2, type, 1, type, 0); 

	if	(func == NULL)
	{
		reg_set(1, type2str(type));
		errmsg(MSG_MDMAT, 16);
	}

	return func;
}

#if	0
Func_t *md_binop(const char *op, const Type_t *t1, const Type_t *t2)
{
	Func_t *func;

	func = GetFunc(NULL, GetGlobalFunc(op), 2, t1, 0, t2, 0); 

	if	(func == NULL)
	{
		reg_cpy(1, op);
		reg_set(2, type2str(t1));
		reg_set(3, type2str(t2));
		liberror(MSG_MDMAT, 18);
	}

	return func;
}

Func_t *md_assignop(const char *op, const Type_t *t1, const Type_t *t2)
{
	Func_t *func;

	func = GetFunc(NULL, GetTypeFunc(t1, op), 2, t1, 1, t2, 0); 

	if	(func == NULL)
	{
		reg_cpy(1, op);
		reg_set(2, type2str(t1));
		reg_set(3, type2str(t2));
		liberror(MSG_MDMAT, 19);
	}

	return func;
}
#endif
