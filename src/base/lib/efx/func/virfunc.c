/*	Virtuelle Funktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/refdata.h>

#define	VIRFUNC_BSIZE	16

static ALLOCTAB(tab_virfunc, 0, sizeof(VirFunc_t));

static VirFunc_t *virfunc_admin(VirFunc_t *tg, const VirFunc_t *src)
{
	if	(tg)
	{
		int i;
		Func_t **func;

		func = tg->tab.data;

		for (i = 0; i < tg->tab.used; i++)
			rd_deref(func[i]);

		vb_free(&tg->tab);
		del_data(&tab_virfunc, tg);
		return NULL;
	}
	else	return new_data(&tab_virfunc);
}


static char *virfunc_ident(VirFunc_t *func)
{
	return func->tab.used ? rd_ident(func->tab.data) : NULL;
}

REFTYPE(VirFuncRefType, "VirFunc", virfunc_ident, virfunc_admin);

VirFunc_t *VirFunc(Func_t *func)
{
	VirFunc_t *vf;

	if	(func && func->reftype == &VirFuncRefType)
		return rd_refer(func);

	vf = rd_create(&VirFuncRefType);
	vb_init(&vf->tab, VIRFUNC_BSIZE, sizeof(Func_t *));

	if	(func && func->reftype == &FuncRefType)
		Val_func(vb_next(&vf->tab)) = func;

	return vf;
}
