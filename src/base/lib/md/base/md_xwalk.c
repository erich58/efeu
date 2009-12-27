/*	Werte einer Datenmatrix durchwandern
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

static void do_walk (mdmat *md, int mode, mdaxis *x, char *data, char *base,
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
		else	do_walk(md, MDXFLAG_MARK, md->axis, data, base, visit);

		return;
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

		do_walk(md, mode, x->next, data + i * x->size, base, visit);
	}
}

void md_xwalk (mdmat *md, const char *def,
	void (*visit)(EfiType *type, void *data, void *base))
{
	if	(!md || !visit)	return;

	md_setflag(md, def, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	do_walk(md, 0, md->axis, md->data, NULL, visit);
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
}
