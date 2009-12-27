/*	Variable eintragen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>

#define	KEY_THIS	"this"

static Obj_t *getvar(Var_t *var, VarTab_t *tab, const Obj_t *obj)
{
	if	(tab == NULL)	return NULL;

	if	(obj && mstrcmp(var->name, KEY_THIS) == 0)
		return RefObj(obj);

	var = xsearch(&tab->tab, var, XS_FIND);
	return var ? Var2Obj(var, obj) : NULL;	
}


Obj_t *GetVar (VarTab_t *tab, const char *name, const Obj_t *obj)
{
	Var_t key;
	Obj_t *x;
	VarStack_t *stack;

	if	(name == NULL)	return NULL;

	key.name = (char *) name;

	if	(tab)
		return getvar(&key, tab, obj);

	if	((x = getvar(&key, LocalVar, obj ? obj : LocalObj)))
		return x;

	for (stack = VarStack; stack != NULL; stack = stack->next)
		if ((x = getvar(&key, stack->tab, obj ? obj : stack->obj)))
			return x;

	return NULL;
}


void AddVar (VarTab_t *tab, Var_t *def, size_t dim)
{
	int i;
	Var_t *old;

	if	(tab == NULL)
	{
		if	(LocalVar == NULL)
			LocalVar = VarTab(NULL, 0);

		tab = LocalVar;
	}

	for (i = 0; i < dim; i++)
	{
		if	(def->name == NULL)	continue;

		if	((old = xsearch(&tab->tab, def + i, XS_REPLACE)))
		{
			DelVar(old);
			reg_cpy(1, def[i].name);
			errmsg(MSG_EFMAIN, 154);
		}
	}
}
