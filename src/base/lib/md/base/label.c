/*	Label abfragen
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mdmat.h>

void MF_label(Func_t *func, void *rval, void **arg)
{
	ObjList_t *list, **ptr;
	Obj_t *obj;
	mdmat_t *md;
	mdaxis_t *x;
	int i;
	int flag;

	md = Val_mdmat(arg[0]);

	if	(md == NULL || md->axis == NULL)
	{
		Val_list(rval) = NULL;
		return;
	}

	list = NULL;
	ptr = &list;
	md_setflag(md, Val_str(arg[1]), 0, mdsf_mark, MDXFLAG_TEMP,
		mdsf_lock, MDFLAG_TEMP);
	flag = Val_bool(arg[2]);

	for (x = md->axis; x != NULL; x = x->next)
	{
		if	(!(x->flags & MDXFLAG_TEMP))	continue;

		for (i = 0; i < x->dim; i++)
		{
			if	(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP))
				continue;

			if	(flag)
			{
				obj = NewPtrObj(&Type_str, msprintf("%s=%s",
					x->name, x->idx[i].name));
			}
			else	obj = LvalObj(&Type_str, md, &x->idx[i].name);

			*ptr = NewObjList(obj);
			ptr = &(*ptr)->next;
		}
	}

	md_allflag(md, 0, mdsf_clear, MDXFLAG_TEMP, mdsf_clear, MDFLAG_TEMP);
	Val_list(rval) = list;
}
