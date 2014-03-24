/*
Virtuelle Funktionen

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
#include <EFEU/refdata.h>

#define	VIRFUNC_BSIZE	16

static ALLOCTAB(tab_virfunc, "EfiVirFunc", 0, sizeof(EfiVirFunc));

static void virfunc_clean (void *data)
{
	EfiVirFunc *tg = data;
	int i;
	EfiFunc **func;

	func = tg->tab.data;

	for (i = 0; i < tg->tab.used; i++)
		rd_deref(func[i]);

	vb_free(&tg->tab);
	del_data(&tab_virfunc, tg);
}


static char *virfunc_ident (const void *data)
{
	const EfiVirFunc *func = data;
	return func->tab.used ?
		rd_ident(((EfiFunc **) func->tab.data)[0]) : NULL;
}

static RefType VirFuncRefType = REFTYPE_INIT("VirFunc",
	virfunc_ident, virfunc_clean);

int IsVirFunc (void *ptr)
{
	EfiVirFunc *func = ptr;
	return (func && func->reftype == &VirFuncRefType);
}

EfiVirFunc *VirFunc (EfiFunc *func)
{
	EfiVirFunc *vf;

	if	(IsVirFunc(func))
		return rd_refer(func);

	vf = new_data(&tab_virfunc);
	vb_init(&vf->tab, VIRFUNC_BSIZE, sizeof(EfiFunc *));

	if	(IsFunc(func))
		Val_func(vb_next(&vf->tab)) = func;

	return rd_init(&VirFuncRefType, vf);
}
