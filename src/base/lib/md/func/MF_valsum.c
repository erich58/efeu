/*	Datenwerte in Liste konvertieren
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

typedef struct {
	EfiFunc *add;
	EfiObj *obj;
} SUMPAR;

static void mainwalk(SUMPAR *sum, mdaxis *x, char *data)
{
	if	(x)
	{
		for (size_t i = 0; i < x->dim; ++i)
		{
			if	(x->idx[i].flags & (MDFLAG_LOCK|MDFLAG_TEMP))
				continue;

			mainwalk(sum, x->next, data + i * x->size);
		}
	}
	else
	{
		CallVoidFunc(sum->add, sum->obj->data, data);
	}
}

void MF_valsum(EfiFunc *func, void *rval, void **arg)
{
	mdmat *md = Val_mdmat(arg[0]);

	if	(!md)	return;

	SUMPAR data;
	data.obj = NewObj(md->type, NULL);
	data.add = mdfunc_add(md->type);

	if	(data.add)
	{
		md_setlock(md, Val_str(arg[1]), 0, MDFLAG_TEMP);
		mainwalk(&data, md->axis, md->data);
		md_clrflag(md, 0, MDFLAG_TEMP);
	}

	Val_ptr(rval) = data.obj;
}
