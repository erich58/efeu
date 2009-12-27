/*	Label abfragen
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

void MF_label(EfiFunc *func, void *rval, void **arg)
{
	EfiObjList *list, **ptr;
	EfiObj *obj;
	mdmat *md;
	mdaxis *x;
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

			obj = NewPtrObj(&Type_str, mstrpaste("=",
				flag ? StrPool_get(x->sbuf, x->i_name) : NULL,
				StrPool_get(x->sbuf, x->idx[i].i_name)));
			*ptr = NewObjList(obj);
			ptr = &(*ptr)->next;
		}
	}

	md_allflag(md, 0, mdsf_clear, MDXFLAG_TEMP, mdsf_clear, MDFLAG_TEMP);
	Val_list(rval) = list;
}
