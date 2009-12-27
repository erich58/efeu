#include "CubeHandle.h"
#include <EFEU/printobj.h>
#include <EFEU/MatchPar.h>

#define	HEAD	"#column;active;desc\n"
				
static void show_choice (CubeHandle *handle, mdaxis *x)
{
	int i;

	for (i = 0; i < x->lbl->cols; i++)
	{
		io_printf(handle->out, "%d;%d;", i, i == x->lbl->idx ? 1 : 0);
		CubeHandle_string(handle,
			StrPool_get(x->lbl->sbuf, x->lbl->tab[i]));
		io_putc('\n', handle->out);
	}
}

static void do_choice (mdaxis *x, char **list, size_t dim)
{
	MatchPar *mp;
	size_t n;

	mdx_index(x, 0);
	mp = MatchPar_vec(list, dim, x->dim);

	for (n = 1; n < x->lbl->cols; n++)
	{
		char *p = StrPool_get(x->lbl->sbuf, x->lbl->tab[n]);

		if	(MatchPar_exec(mp, p, n))
		{
			x->lbl->idx = n;
			break;
		}
	}

	rd_deref(mp);
}

void CubeHandle_choice (CubeHandle *handle, char *def)
{
	MatchPar *mp;
	mdlist *l, *list;
	mdaxis *x;
	int dim, n;
	int need_head;

	dim = md_dim(handle->md->axis);
	list = str2mdlist(def, 0);
	need_head = 1;

	for (l = list; l; l = l->next)
	{
		mp = MatchPar_create(l->name, dim);

		for (x = handle->md->axis, n = 1; x; x = x->next, n++)
		{
			if	(MatchPar_exec(mp, StrPool_get(x->sbuf,
					x->i_name), n))
			{
				do_choice(x, l->list, l->dim);

				if	(need_head && handle->headline)
					io_puts(HEAD, handle->out);

				need_head = 0;
				show_choice(handle, x);
				break;
			}
		}

		rd_deref(mp);
	}

	del_mdlist(list);
}
