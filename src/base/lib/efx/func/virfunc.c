/*
Virtuelle Funktionen

$Copyright (C) 1994 Erich Fr�hst�ck
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
	return func->tab.used ?
		rd_ident(((Func_t **) func->tab.data)[0]) : NULL;
}

ADMINREFTYPE(VirFuncRefType, "VirFunc", virfunc_ident, virfunc_admin);

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
