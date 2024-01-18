/*	Achsenflags setzen/löschen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/MatchPar.h>

#define	INVERT_FLAG	0x1100

unsigned mdsf_mark (unsigned old, int sel, unsigned val)
{
	return sel ? (old | val) : (old & ~val);
}

unsigned mdsf_clear (unsigned old, int sel, unsigned val)
{
	return sel ? (old & ~val) : old;
}

unsigned mdsf_lock (unsigned old, int sel, unsigned val)
{
	return sel ? (old & ~val) : (old | val);
}

unsigned mdsf_toggle (unsigned old, int sel, unsigned val)
{
	if	(sel)
	{
		return (old & val) ? (old & ~val) : (old | val);
	}
	else	return old;
}

static void subfunc (mdaxis *x, mdlist *l,
	unsigned (*func) (unsigned old, int flag, unsigned val), unsigned flag);


static void allindex (mdaxis *x,
	unsigned (*func) (unsigned old, int flag, unsigned val), unsigned flag)
{
	int n;

	for (n = 0; n < x->dim; n++)
		x->idx[n].flags = func(x->idx[n].flags, 1, flag);
}

static void subfunc (mdaxis *x, mdlist *l,
	unsigned (*func) (unsigned old, int flag, unsigned val), unsigned flag)
{
	MatchPar *mp;
	int n;

	mp = MatchPar_vec(l->list, l->dim, x->dim);

	for (n = 0; n < x->dim; n++)
		x->idx[n].flags = func(x->idx[n].flags,
			MatchPar_exec(mp, StrPool_get(x->sbuf,
				x->idx[n].i_name), n + 1), flag);

	rd_deref(mp);
}


void md_allflag (mdmat *md,
	unsigned (*fx) (unsigned old, int flag, unsigned val), unsigned vx,
	unsigned (*fi) (unsigned old, int flag, unsigned val), unsigned vi)
{
	mdaxis *x;

	if	(md == NULL)	return;

	for (x = md->axis; x != NULL; x = x->next)
	{
		if	(fx)	x->flags = fx(x->flags, 1, vx);
		if	(fi)	allindex(x, fi, vi);
	}
}


/*	Hauptfunktion
*/

void md_setflag (mdmat *md, const char *def,
	unsigned (*fx) (unsigned old, int flag, unsigned val), unsigned vx,
	unsigned (*fi) (unsigned old, int flag, unsigned val), unsigned vi)
{
	mdlist *l, *list;
	mdaxis *x;
	MatchPar *mp;
	int n, dim;

	if	(md == NULL || md->axis == NULL)
		return;

/*	Achsenflags initialisieren
*/
	if	(fx)
	{
		for (x = md->axis; x != NULL; x = x->next)
			x->flags = fx(x->flags, 0, vx);
	}

/*	Universielle Selektion
*/
	if	(def == NULL)
	{
		if	(fi != 0)
		{
			for (x = md->axis; x != NULL; x = x->next)
				allindex(x, fi, vi);
		}
		return;
	}

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
				if	(fx)	x->flags = fx(x->flags, 1, vx);

				if	(fi == NULL)
					;
				else if	(l->dim)
					subfunc(x, l, fi, vi);
				else
					allindex(x, fi, vi);
			}
		}

		rd_deref(mp);
	}

	del_mdlist(list);
}

static mdmat *md_axis_flag(mdmat *md, const char *def, int mask)
{
	if	(!md || !md->axis)
		return md;

	if	(def == NULL)
	{
		for (mdaxis *x = md->axis; x; x = x->next)
			x->flags &= ~mask;

		return md;
	}

	unsigned (*fx)(unsigned old, int sel, unsigned val) = mdsf_mark;

	if	(*def == '~')
		fx = mdsf_toggle, def++;
	else if	(*def == '!')
		fx = mdsf_clear, def++;

	md_setflag(md, def, fx, mask, NULL, 0);
	return md;
}

mdmat *md_mark(mdmat *md, const char *def)
{
	return md_axis_flag(md, def, MDXFLAG_MARK);
}

mdmat *md_time(mdmat *md, const char *def)
{
	return md_axis_flag(md, def, MDXFLAG_TIME);
}

mdmat *md_hide(mdmat *md, const char *def)
{
	return md_axis_flag(md, def, MDXFLAG_HIDE);
}

mdmat *md_temp(mdmat *md, const char *def)
{
	return md_axis_flag(md, def, MDXFLAG_TEMP);
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

				if	(x->idx[n].flags & MDFLAG_LOCK)
					sb_putc('l', &buf);
				if	(x->idx[n].flags & MDFLAG_BASE)
					sb_putc('b', &buf);
				if	(x->idx[n].flags & MDFLAG_HIDE)
					sb_putc('h', &buf);
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
