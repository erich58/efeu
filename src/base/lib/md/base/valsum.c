/*	Datenwerte in Liste konvertieren
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdeval.h>

typedef struct {
	EfiFunc *add;
	EfiType *type;
	void *data;
} SUMPAR;

static void *sum_init(void *xpar, EfiType *type)
{
	SUMPAR *par;

	par = memalloc(sizeof(SUMPAR) + type->size);
	par->data = (par + 1);
	memset(par->data, 0, type->size);
	par->add = mdfunc_add(type);
	par->type = type;
	return par;
}

static void *sum_end(void *par)
{
	EfiObj *obj;

	obj = NewObj(((SUMPAR *) par)->type, ((SUMPAR *) par)->data);
	memfree(par);
	return obj;
}

static void sum_data(void *par, EfiType *type, void *ptr, void *base)
{
	CallVoidFunc(((SUMPAR *) par)->add, ((SUMPAR *) par)->data, ptr);
}

static MdEvalDef sum_eval = {
	sum_init, sum_end,
	NULL, NULL,
	NULL, NULL,
	sum_data,
};


void MF_valsum(EfiFunc *func, void *rval, void **arg)
{
	register mdmat *md = Val_mdmat(arg[0]);
	md_setflag(md, Val_str(arg[1]), 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
	Val_ptr(rval) = md_eval(&sum_eval, NULL, md, MDFLAG_LOCK|MDFLAG_TEMP, 0, 0);
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
}
