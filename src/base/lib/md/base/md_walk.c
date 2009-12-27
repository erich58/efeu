/*	Werte einer Datenmatrix durchwandern
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static void do_walk (mdmat *md, int mode, mdaxis *x, char *data,
	void (*visit)(EfiType *type, void *data))
{
	int i;

	while (x && (x->flags & MDXFLAG_MARK) != mode)
		x = x->next;

	if	(x == NULL)
	{
		if	(mode)
		{
			visit(md->type, data);
		}
		else	do_walk(md, MDXFLAG_MARK, md->axis, data, visit);

		return;
	}

	for (i = 0; i < x->dim; i++)
		if (!(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP)))
			do_walk(md, mode, x->next, data + i * x->size, visit);
}

void md_walk (mdmat *md, const char *def,
	void (*visit)(EfiType *type, void *data))
{
	if	(!md || !visit)	return;

	md_setflag(md, def, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	do_walk(md, 0, md->axis, md->data, visit);
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
}
