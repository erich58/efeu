/*	Werte einer Datenmatrix durchwandern
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static int do_walk (void *par, mdmat *md, int mode, mdaxis *x, char *data,
	int (*visit)(void *par, EfiType *type, void *data))
{
	size_t i;
	int stat;

	while (x && (x->flags & MDXFLAG_MARK) != mode)
		x = x->next;

	if	(x == NULL)
	{
		if	(mode)
			return visit(par, md->type, data);

		return do_walk(par, md, MDXFLAG_MARK, md->axis, data, visit);
	}

	for (i = 0; i < x->dim; i++)
	{
		if	((x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP)))
			continue;

		stat = do_walk(par, md, mode, x->next,
			data + i * x->size, visit);

		if	(stat)	return stat;
	}

	return 0;
}

int md_walk (void *par, mdmat *md, const char *def,
	int (*visit)(void *par, EfiType *type, void *data))
{
	int stat;

	if	(!md || !visit)	return 0;

	md_setflag(md, def, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	stat = do_walk(par, md, 0, md->axis, md->data, visit);
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
	return stat;
}
