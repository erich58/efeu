/*	Datenmatrix generieren
	(c) 2024 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static void make_axis(mdmat *md, mdlist *list)
{
	if	(!list)	return;

	mdlist *l = list;

	if	(l->dim == 0)
	{
		md->i_name = StrPool_xadd(md->sbuf, l->name);
		l = l->next;
	}

	mdaxis **ptr = &md->axis;

	while (l)
	{
		mdaxis *x = new_axis(md->sbuf, l->dim ? l->dim : 1);
		x->i_name = StrPool_xadd(x->sbuf, l->name);

		if	(!l->dim)
			x->idx[0].i_name = x->i_name;

		for (size_t i = 0; i < l->dim; ++i)
			x->idx[i].i_name = StrPool_add(x->sbuf, l->list[i]);

		*ptr = x;
		ptr = &(*ptr)->next;
		l = l->next;
	}

	del_mdlist(list);
}

mdmat *md_create (EfiType *type, const char *def)
{
	if	(type == NULL)
		return NULL;

	mdmat *md = new_mdmat(); 
	md->type = type;
	md->sbuf = NewStrPool();
	make_axis(md, str2mdlist(def, 0));
	md_alloc(md);
	return md;
}
