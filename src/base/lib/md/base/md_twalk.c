/*	Werte einer Datenmatrix mit Zeitachse durchwandern
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static int do_walk (void *par, mdmat *md, int mode, int lag,
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

		return do_walk(par, md, MDXFLAG_MARK, lag,
			md->axis, data, base, visit);
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

		stat = do_walk(par, md, mode, lag, x->next,
			data + i * x->size, ptr, visit);

		if	(stat)	return stat;
	}

	return 0;
}

int md_twalk (void *par, mdmat *md, const char *def, int lag,
	int (*visit)(void *par, EfiType *type, void *data, void *base))
{
	int stat;

	if	(!md || !visit)	return 0;

	md_setflag(md, def, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	stat = do_walk(par, md, 0, lag, md->axis, md->data, NULL, visit);
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
	return stat;
}
