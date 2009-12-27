/*
Datenmatrix reorganisieren
*/

#include <EFEU/mdmat.h>

void md_reorg_label (mdmat *md)
{
	StrPool *sbuf;
	mdaxis *x;
	mdindex *idx;
	size_t n;

	if	(!md)	return;

	sbuf = NewStrPool();
	md->i_name = StrPool_copy(sbuf, md->sbuf, md->i_name);
	md->i_desc = StrPool_copy(sbuf, md->sbuf, md->i_desc);

	for (x = md->axis; x; x = x->next)
	{
		x->i_name = StrPool_copy(sbuf, x->sbuf, x->i_name);
		x->i_desc = StrPool_copy(sbuf, x->sbuf, x->i_desc);

		for (n = x->dim, idx = x->idx; n-- > 0; idx++)
		{
			idx->i_name = StrPool_copy(sbuf, x->sbuf, idx->i_name);
			idx->i_desc = StrPool_copy(sbuf, x->sbuf, idx->i_desc);
		}

		rd_deref(x->sbuf);
		x->sbuf = rd_refer(sbuf);
	}

	rd_deref(md->sbuf);
	md->sbuf = sbuf;
}
