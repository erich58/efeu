/*	Zeilenweise Datenauswertung
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mdmat.h>

typedef struct {
	Func_t *func;
	char *label;
	ObjList_t *list;
	ObjList_t **ptr;
} LEVAL;

#define	PAR	((LEVAL *) (par))

static void *leval_init(void *op, Type_t *type)
{
	LEVAL *par;

	par = memalloc(sizeof(LEVAL));
	par->func = GetFunc(NULL, op, 2, &Type_str, 0, &Type_list, 0);
	return par;
}

static void *leval_newidx(const void *idx, char *name)
{
	return mstrpaste(".", idx, name);
}

static void leval_start(void *par, const void *idx)
{
	PAR->label = (void *) idx;
	PAR->list = NULL;
	PAR->ptr = &PAR->list;
}

static void leval_end(void *par)
{
	CallVoidFunc(PAR->func, &PAR->label, &PAR->list);
	DelObjList(PAR->list);
}

static void leval_data(void *par, Type_t *type, void *ptr, void *base)
{
	*PAR->ptr = NewObjList(LvalObj(type, NULL, ptr));
	PAR->ptr = &(*PAR->ptr)->next;
}

static mdeval_t leval_eval = {
	leval_init, NULL,
	leval_newidx, memfree,
	leval_start, leval_end,
	leval_data,
};


void MF_leval(Func_t *func, void *rval, void **arg)
{
	mdmat_t *md;

	md = Val_mdmat(arg[0]);
	md_setflag(md, Val_str(arg[2]), 0, mdsf_mark, MDXFLAG_MARK, mdsf_lock, MDFLAG_TEMP);
	md_eval(&leval_eval, Val_vfunc(arg[1]), md, MDFLAG_LOCK|MDFLAG_TEMP, 0, 0);
	md_allflag(md, 0, mdsf_clear, MDXFLAG_MARK, mdsf_clear, MDFLAG_TEMP);
	Val_mdmat(rval) = ref_mdmat(md);
}
