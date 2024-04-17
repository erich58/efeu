/*	Werte einer Datenmatrix mit Zeitachse durchwandern
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static int do_walk (mdmat *md, int mode, int lag,
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

		return do_walk(md, MDXFLAG_MARK, lag,
			md->axis, data, base, visit, par);
	}

	flag = (x->flags & MDXFLAG_TIME);

	for (i = 0; i < x->dim; i++)
	{
		if	(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP))
			continue;

		if	(flag)
		{
			int k = i - lag;

			if	(k >= 0 && k < x->dim)
			{
				ptr = data + k * x->size;
			}
			else	ptr = NULL;
		}
		else if	(base)
		{
			ptr = base + i * x->size;
		}
		else	ptr = NULL;

		stat = do_walk(md, mode, lag, x->next,
			data + i * x->size, ptr, visit, par);

		if	(stat)	return stat;
	}

	return 0;
}

int md_twalk (mdmat *md, const char *def, int lag, MdVisit visit, void *par)
{
	int stat;

	if	(!md || !visit)	return 0;

	md_setlock(md, def, 0, MDFLAG_TEMP);
	stat = do_walk(md, 0, lag, md->axis, md->data, NULL, visit, par);
	md_clrflag(md, 0, MDFLAG_TEMP);
	return stat;
}
