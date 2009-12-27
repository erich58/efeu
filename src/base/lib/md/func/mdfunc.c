/*	MDMAT - Funktionen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/object.h>
#include <EFEU/mdmat.h>


#define	RVMD	Val_mdmat(rval)
#define	STR(n)	Val_str(arg[n])
#define	MD(n)	Val_mdmat(arg[n])

#define	REF(ptr)	(rd_refer(Val_mdmat(ptr)))
#define	REFRETVAL	RVMD = rd_refer(Val_mdmat(arg[0]))


#define	MDB_FUNC(name, expr)	\
static void name(Func_t *func, void *rval, void **arg)	\
{ register mdmat_t *md = rd_refer(Val_mdmat(arg[0]));	\
expr; Val_mdmat(rval) = md; }

#define	MDC_FUNC(name, expr)	\
static void name(Func_t *func, void *rval, void **arg)	\
{ Val_mdmat(rval) = expr; }

MDC_FUNC(MF_load, md_fload(STR(0), STR(1), STR(2)))
MDC_FUNC(MF_read, md_read(Val_io(arg[0]), STR(1)))
MDC_FUNC(MF_reload, md_reload(MD(0), STR(1), STR(2)))

/*
static void MF_reload(Func_t *func, void *rval, void **arg)
{
	io_t *io;

	io = io_tmpfile();
	md_save(io, MD(0), MDFLAG_LOCK);
	io_rewind(io);
	RVMD = md_load(io, STR(1), STR(2));
	io_close(io);
}
*/

static void MF_cpy(Func_t *func, void *rval, void **arg)
{
	register mdmat_t *md = MD(0);
	register char *def = STR(1);

	if	(md && def)
	{
		md_setflag(md, def, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
		RVMD = cpy_mdmat(md, MDFLAG_TEMP);
		md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
	}
	else	RVMD = cpy_mdmat(md, 0);
}

static void MF_paste (Func_t *func, void *rval, void **arg)
{
	register mdmat_t *md = Val_mdmat(arg[0]);

	if	(md != NULL)
	{
		char *name, *delim;
		int p, n, dim;

		name = STR(1);
		delim = STR(4);
		dim = md_dim(md->axis);
		p = Val_int(arg[2]);
		n = Val_int(arg[3]);

		if	(n <= 0 || p + n > dim)
			n = dim - p;

		md_paste(md, name, delim, p, n);
		md->size = md_size(md->axis, md->type->size);
	}

	Val_mdmat(rval) = rd_refer(md);
}

MDC_FUNC(MF_mdmul, md_mul(MD(0), MD(1), Val_int(arg[2])))
/*
MDC_FUNC(MF_mdinv, md_inv(MD(0)))
*/
MDC_FUNC(MF_mdprod, md_prod(MD(0)))
MDB_FUNC(MF_save, md_fsave(STR(1), MD(0), Val_bool(arg[2]) ? MDFLAG_LOCK : 0))
MDB_FUNC(MF_print, md_print(rd_refer(Val_io(arg[1])), MD(0), STR(2)))

MDB_FUNC(MF_konv, md_konv(md, Val_type(arg[1])))
MDB_FUNC(MF_round, md_round(md, Val_double(arg[1]), MDFLAG_LOCK))
MDB_FUNC(MF_adjust, md_adjust(md, Val_double(arg[1]), MDFLAG_LOCK))
MDB_FUNC(MF_korr, md_korr(md, Val_bool(arg[1])))

MDB_FUNC(MF_reduce, md_reduce(md, STR(1)))
MDB_FUNC(MF_permut, md_permut(md, STR(1)))

MDC_FUNC(MF_sumaxis, md_sum(MD(0), STR(1)))
MDC_FUNC(MF_diag, md_diag(MD(0), STR(1)))

MDB_FUNC(MF_selectall, md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_LOCK))
MDB_FUNC(MF_select, md_setflag(md, STR(1), 0, NULL, 0, mdsf_lock, MDFLAG_LOCK))
MDB_FUNC(MF_clrbase, md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_BASE))
MDB_FUNC(MF_base,
	md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_BASE);
	md_setflag(md, STR(1), 0, NULL, 0, mdsf_mark, MDFLAG_BASE)
)
MDB_FUNC(MF_mark, md_setflag(md, STR(1), 0, mdsf_mark, MDXFLAG_MARK, NULL, 0))
MDB_FUNC(MF_transp, md_allflag(md, 0, mdsf_toggle, MDXFLAG_MARK, NULL, 0))
MDB_FUNC(MF_time, md_setflag(md, STR(1), 0, mdsf_mark, MDXFLAG_TIME, NULL, 0))
MDB_FUNC(MF_hide, md_setflag(md, STR(1), 0, mdsf_mark, MDXFLAG_HIDE, NULL, 0))

MDB_FUNC(MF_assign, md_assign(func->name, MD(0), MD(1)))

MDB_FUNC(MF_expr, md_assign(Val_str(arg[1]), md,
	func->dim > 2 ? MD(2) : NULL))

MDC_FUNC(MF_mdexpr, md_term(Val_vfunc(arg[0]), MD(1),
	func->dim > 2 ? MD(2) : NULL))

MDC_FUNC(MF_term, md_term(GetGlobalFunc(func->name), MD(0), MD(1)))

static void MF_cat(Func_t *func, void *rval, void **arg)
{
	mdmat_t **tab;
	mdmat_t *md;
	ObjList_t *list;
	Obj_t *obj;
	size_t i, dim;

	list = Val_list(arg[1]);

	if	(list == NULL)
	{
		RVMD = NULL;
		return;
	}

	dim = ObjListLen(list);
	tab = ALLOC(dim, mdmat_t *);

	for (i = 0; list != NULL; list = list->next)
	{
		obj = EvalObj(RefObj(list->obj), &Type_mdmat);
		tab[i++] = obj ? ref_mdmat(Val_mdmat(obj->data)) : NULL;
		UnrefObj(obj);
	}

	md = md_cat(Val_str(arg[0]), tab, dim);

	for (i = 0; i < dim; i++)
		rd_deref(tab[i]);

	memfree(tab);
	RVMD = md;
}

static void MF_index(Func_t *func, void *rval, void **arg)
{
	mdmat_t *md = Val_mdmat(arg[0]);
	int n = Val_int(arg[1]);

	if	(md && n * md->type->size < md->size)
	{
		Val_obj(rval) = LvalObj(&Lval_ref, md->type, md,
			(char *) md->data + n * md->type->size);
	}
	else	Val_obj(rval) = NULL;
}

static void MF_md2vec(Func_t *func, void *rval, void **arg)
{
	mdmat_t *md = Val_mdmat(arg[0]);
	Vec_t *vec = rval;

	if	(md != NULL)
	{
		vec->type = md->type;
		vec->dim = md->size / md->type->size;
		vec->data = md->data;
	}
	else	memset(vec, 0, sizeof(Vec_t));
}

extern void *md_DATA2(mdmat_t *md, unsigned mask, unsigned base, int lag);

extern void MF_valsum(Func_t *func, void *rval, void **arg);
extern void MF_leval(Func_t *func, void *rval, void **arg);
extern void MF_xeval(Func_t *func, void *rval, void **arg);

static FuncDef_t fdef[] = {
	{ 0, &Type_mdmat, "mdload (str file, str sel = NULL, str vsel = NULL)",
		MF_load },
	{ 0, &Type_mdmat, "mdread (IO io, str def = NULL)", MF_read },
	{ 0, &Type_mdmat, "reload (mdmat md, str sel = NULL, str vsel = NULL)",
		MF_reload },
	{ 0, &Type_mdmat, "mdmul (mdmat a, mdmat b, int flag = 2)", MF_mdmul },
/*
	{ 0, &Type_mdmat, "mdinv (mdmat a)", MF_mdinv },
*/
	{ 0, &Type_mdmat, "mdprod (mdmat a)", MF_mdprod },
	{ 0, &Type_mdmat, "mdcpy (mdmat, str def = NULL)", MF_cpy },
	{ 0, &Type_mdmat, "sumaxis (mdmat md, str list)", MF_sumaxis },
	{ 0, &Type_mdmat, "mddiag (mdmat md, str list)", MF_diag },
	{ FUNC_VIRTUAL, &Type_mdmat, "mdexpr (VirFunc op, mdmat, mdmat)",
		MF_mdexpr },
	{ FUNC_VIRTUAL, &Type_mdmat, "mdexpr (VirFunc op, mdmat)", MF_mdexpr },
	{ 0, &Type_mdmat, "mdcat (str def, ...)", MF_cat },

	{ 0, &Type_mdmat, "mdmat::operator+= (mdmat)", MF_assign },
	{ 0, &Type_mdmat, "mdmat::operator-= (mdmat)", MF_assign },
	{ 0, &Type_mdmat, "mdmat::operator*= (mdmat)", MF_assign },
	{ 0, &Type_mdmat, "mdmat::operator/= (mdmat)", MF_assign },

	{ FUNC_RESTRICTED, &Type_vec, "mdmat ()", MF_md2vec },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (mdmat, int)", MF_index },
	{ FUNC_VIRTUAL, &Type_mdmat, "operator+ (mdmat, mdmat)", MF_term },
	{ FUNC_VIRTUAL, &Type_mdmat, "operator- (mdmat, mdmat)", MF_term },
	{ FUNC_VIRTUAL, &Type_mdmat, "operator* (mdmat, mdmat)", MF_term },
	{ FUNC_VIRTUAL, &Type_mdmat, "operator/ (mdmat, mdmat)", MF_term },

	{ FUNC_VIRTUAL, &Type_mdmat, "mdmat::expr (str op, mdmat)", MF_expr },
	{ FUNC_VIRTUAL, &Type_mdmat, "mdmat::expr (str op)", MF_expr },

	{ 0, &Type_mdmat, "mdmat::save (str file, bool select = true)",
		MF_save },
	{ 0, &Type_mdmat, "mdmat::print (IO io = iostd, str def = NULL)",
		MF_print },
	{ 0, &Type_mdmat, "mdmat::konv (Type_t type)", MF_konv },
	{ 0, &Type_mdmat, "mdmat::round (double factor = 1.)", MF_round },
	{ 0, &Type_mdmat, "mdmat::adjust (double val = 0.)", MF_adjust },
	{ 0, &Type_mdmat, "mdmat::korr (bool round = false)", MF_korr },
	{ 0, &Type_mdmat, "mdmat::reduce (str keep = NULL)", MF_reduce },
	{ 0, &Type_mdmat, "mdmat::permut (str list)", MF_permut },
	{ 0, &Type_mdmat, "mdmat::paste (str name, int pos, int n, str delim = \".\")",
		MF_paste },

	{ FUNC_VIRTUAL, &Type_mdmat, "mdmat::select ()", MF_selectall },
	{ FUNC_VIRTUAL, &Type_mdmat, "mdmat::select (str def)", MF_select },
	{ FUNC_VIRTUAL, &Type_mdmat, "mdmat::base ()", MF_clrbase },
	{ FUNC_VIRTUAL, &Type_mdmat, "mdmat::base (str def)", MF_base },
	{ 0, &Type_mdmat, "mdmat::mark (str def = NULL)", MF_mark },
	{ 0, &Type_mdmat, "mdmat::transp ()", MF_transp },
	{ 0, &Type_mdmat, "mdmat::time (str def = NULL)", MF_time },
	{ 0, &Type_mdmat, "mdmat::hide (str def = NULL)", MF_hide },
	{ 0, &Type_list, "mdmat::data (str def = NULL, VirFunc idx = NULL,\
int lag = 0)", MF_data },
	{ 0, &Type_list, "mdmat::label (str def = NULL, bool flag = false)", MF_label },

	{ FUNC_VIRTUAL, &Type_obj,  "sum (mdmat, str = NULL)", MF_valsum },
	{ 0, &Type_mdmat, "mdmat::leval (VirFunc op, str def = NULL)",
		MF_leval },
	{ 0, &Type_mdmat, "mdmat::eval (str axis, Expr_t cmd)", MF_xeval },
};


void MdFuncDef(void)
{
	AddFuncDef(fdef, tabsize(fdef));
}
