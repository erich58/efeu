/*	Achsenflags setzen/löschen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/MatchPar.h>

mdmat *md_clrflag(mdmat *md, unsigned mx, unsigned mi)
{
	if	(md)
	{
		for (mdaxis *x = md->axis; x != NULL; x = x->next)
		{
			x->flags &= ~mx;

			if	(mi)
			{
				for (size_t i = 0; i < x->dim; ++i)
					x->idx[i].flags &= ~mi;
			}
		}
	}

	return md;
}

static void allindex (mdaxis *x, unsigned mask)
{
	for (int n = 0; n < x->dim; n++)
		x->idx[n].flags |= mask;
}

static void subfunc (mdaxis *x, mdlist *l, unsigned mask, int flag)
{
	MatchPar *mp = MatchPar_vec(l->list, l->dim, x->dim);

	for (size_t n = 0; n < x->dim; n++)
	{
		const char *name = StrPool_get(x->sbuf, x->idx[n].i_name);

		if	(MatchPar_exec(mp, name, n + 1) == flag)
			x->idx[n].flags |= mask;
	}

	rd_deref(mp);
}


/*	Hauptfunktion
*/

static mdmat *do_setflag (mdmat *md, const char *def,
	unsigned vx, unsigned vi, int flag)
{
	mdlist *l, *list;
	mdaxis *x;
	MatchPar *mp;
	size_t n, dim;

	md_clrflag(md, vx, vi);

	if	(!def)
		return md;

	dim = md_dim(md->axis);
	list = str2mdlist(def, 0);

	for (l = list; l != NULL; l = l->next)
	{
		mp = MatchPar_create(l->name, dim);

		for (x = md->axis, n = 1; x != NULL; x = x->next, n++)
		{
			if	(MatchPar_exec(mp, StrPool_get(x->sbuf,
					x->i_name), n))
			{
				x->flags |= vx;

				if	(vi)
				{
					if	(l->dim)
						subfunc(x, l, vi, flag);
					else if	(flag)
						allindex(x, vi);
				}
			}
		}

		rd_deref(mp);
	}

	del_mdlist(list);
	return md;
}

mdmat *md_setflag(mdmat *md, const char *def, unsigned mx, unsigned mi)
{
	return do_setflag(md, def, mx, mi, 1);
}

mdmat *md_setlock(mdmat *md, const char *def, unsigned mx, unsigned mi)
{
	return do_setflag(md, def, mx, mi, 0);
}

mdmat *md_transp(mdmat *md)
{
	for (mdaxis *x = md ? md->axis : NULL; x != NULL; x = x->next)
	{
		if	(x->flags & MDXFLAG_MARK)
			x->flags &= ~MDXFLAG_MARK;
		else
			x->flags |= MDXFLAG_MARK;
	}

	return md;
}

const char *md_showflag(mdmat *md, int verbose)
{
	static STRBUF(buf, 32);

	if	(!md) return NULL;

	sb_clear(&buf);
	const char *delim = "";

	for (mdaxis *x = md->axis; x; x = x->next)
	{
		sb_puts(delim, &buf);
		sb_puts(StrPool_get(x->sbuf, x->i_name), &buf);

		if (x->flags)
		{
			sb_putc('[', &buf);

			if	(x->flags & MDXFLAG_MARK)
				sb_putc('m', &buf);
			if	(x->flags & MDXFLAG_TIME)
				sb_putc('t', &buf);
			if	(x->flags & MDXFLAG_HIDE)
				sb_putc('h', &buf);
			if	(x->flags & MDXFLAG_TEMP)
				sb_putc('+', &buf);

			sb_putc(']', &buf);
		}

		if	(verbose == 0)
		{
			delim = " ";
			continue;
		}

		delim = "=";

		for (size_t n = 0; n < x->dim; n++)
		{
			sb_puts(delim, &buf);
			delim = ",";
			sb_puts(StrPool_get(x->sbuf, x->idx[n].i_name), &buf);

			if (x->idx[n].flags)
			{
				sb_putc('[', &buf);

				if	(x->idx[n].flags & MDFLAG_BASE)
					sb_putc('B', &buf);
				if	(x->idx[n].flags & MDFLAG_LOCK)
					sb_putc('L', &buf);
				if	(x->idx[n].flags & MDFLAG_HIDE)
					sb_putc('H', &buf);
				if	(x->idx[n].flags & MDFLAG_TEMP)
					sb_putc('-', &buf);

				sb_putc(']', &buf);
			}
		}

		sb_putc('\n', &buf);
		delim = "";
	}

	return sb_cstr(&buf);
}
