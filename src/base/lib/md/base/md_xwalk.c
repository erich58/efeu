/*	Werte einer Datenmatrix durchwandern
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static int do_walk (void *par, mdmat *md, int mode,
	mdaxis *x, char *data, char *base,
	int (*visit)(void *par, EfiType *type, void *data, void *base))
{
	char *ptr;
	int flag;
	size_t i;
	int stat;

	while (x && (x->flags & MDXFLAG_MARK) != mode)
		x = x->next;

	if	(x == NULL)
	{
		if	(mode)
			return visit(par, md->type, data, base);

		return do_walk(par, md, MDXFLAG_MARK,
			md->axis, data, base, visit);
	}

	if	(base)
	{
		ptr = base;
		flag = 1;
	}
	else
	{
		ptr = data;
		flag = 0;
	}

	for (i = 0; i < x->dim; i++)
	{
		if	(x->idx[i].flags & MDFLAG_BASE)
		{
			base = ptr + i * x->size;
			flag = 0;
			break;
		}
	}

	for (i = 0; i < x->dim; i++)
	{
		if	(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP))
			continue;

		if	(flag || (x->idx[i].flags & MDFLAG_BASE))
			base = ptr + i * x->size;

		stat = do_walk(par, md, mode, x->next,
			data + i * x->size, base, visit);

		if	(stat)	return stat;
	}

	return 0;
}

int md_xwalk (void *par, mdmat *md, const char *def,
	int (*visit)(void *par, EfiType *type, void *data, void *base))
{
	int stat;

	if	(!md || !visit)	return 0;

	md_setflag(md, def, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	stat = do_walk(par, md, 0, md->axis, md->data, NULL, visit);
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
	return stat;
}
