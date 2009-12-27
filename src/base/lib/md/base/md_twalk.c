/*	Werte einer Datenmatrix mit Zeitachse durchwandern
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static void do_walk (mdmat *md, int mode, int lag,
	mdaxis *x, char *data, char *base,
	void (*visit)(EfiType *type, void *data, void *base))
{
	char *ptr;
	int flag;
	int i;

	while (x && (x->flags & MDXFLAG_MARK) != mode)
		x = x->next;

	if	(x == NULL)
	{
		if	(mode)
		{
			visit(md->type, data, base);
		}
		else	do_walk(md, MDXFLAG_MARK, lag, md->axis,
				data, base, visit);

		return;
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

		do_walk(md, mode, lag, x->next,
			data + i * x->size, ptr, visit);
	}
}

void md_twalk (mdmat *md, const char *def, int lag,
	void (*visit)(EfiType *type, void *data, void *base))
{
	if	(!md || !visit)	return;

	md_setflag(md, def, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	do_walk(md, 0, lag, md->axis, md->data, NULL, visit);
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
}
