/*	Werte einer Datenmatrix durchwandern
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static int do_walk (mdmat *md, int mode,
	mdaxis *x, char *data, char *base,
	MdVisit visit, void *par)
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
			return visit(md->type, data, base, par);

		return do_walk(md, MDXFLAG_MARK,
			md->axis, data, base, visit, par);
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

		stat = do_walk(md, mode, x->next,
			data + i * x->size, base, visit, par);

		if	(stat)	return stat;
	}

	return 0;
}

int md_walk (mdmat *md, const char *def, const char *base,
	MdVisit visit, void *par)
{
	int stat;

	if	(!md || !visit)	return 0;

	md_setlock(md, def, 0, MDFLAG_TEMP);
	md_base(md, base);
	stat = do_walk(md, 0, md->axis, md->data, NULL, visit, par);
	md_clrflag(md, 0, MDFLAG_TEMP);
	return stat;
}
