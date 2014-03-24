/*	MDMAT - Funktionen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/object.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <EFEU/stdtype.h>
#include <EFEU/MatchPar.h>


#define	RVMD	Val_mdmat(rval)
#define	STR(n)	Val_str(arg[n])
#define	MD(n)	Val_mdmat(arg[n])

#define	REF(ptr)	(rd_refer(Val_mdmat(ptr)))
#define	REFRETVAL	RVMD = rd_refer(Val_mdmat(arg[0]))


#define	MDB_FUNC(name, expr)	\
static void name(EfiFunc *func, void *rval, void **arg)	\
{ register mdmat *md = rd_refer(Val_mdmat(arg[0]));	\
expr; Val_mdmat(rval) = md; }

#define	MDC_FUNC(name, expr)	\
static void name(EfiFunc *func, void *rval, void **arg)	\
{ Val_mdmat(rval) = expr; }

MDC_FUNC(MF_load, md_fload(STR(0), STR(1), STR(2)))
MDC_FUNC(MF_read, md_read(Val_io(arg[0]), STR(1)))
MDC_FUNC(MF_reload, md_reload(rd_refer(MD(0)), STR(1), STR(2)))

static void MF_file (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = md_file(Val_io(arg[0]),
		Val_str(arg[1]), Val_str(arg[2]));
}

static void MF_cpy(EfiFunc *func, void *rval, void **arg)
{
	register mdmat *md = MD(0);
	register char *def = STR(1);

	if	(md && def)
	{
		md_setflag(md, def, 0, NULL, 0, mdsf_lock, MDFLAG_TEMP);
		RVMD = cpy_mdmat(md, MDFLAG_TEMP);
		md_allflag(md, 0, NULL, 0, mdsf_clear, MDFLAG_TEMP);
	}
	else	RVMD = cpy_mdmat(md, 0);
}

static void MF_paste (EfiFunc *func, void *rval, void **arg)
{
	register mdmat *md = Val_mdmat(arg[0]);

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

static void MF_map (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = md_fputmap(Val_mdmat(arg[0]), Val_str(arg[1]));
}

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

static void MF_cat(EfiFunc *func, void *rval, void **arg)
{
	mdmat **tab;
	mdmat *md;
	EfiObjList *list;
	EfiObj *obj;
	size_t i, dim;

	list = Val_list(arg[1]);

	if	(list == NULL)
	{
		RVMD = NULL;
		return;
	}

	dim = ObjListLen(list);
	tab = memalloc(dim * sizeof(mdmat *));

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

static void MF_index(EfiFunc *func, void *rval, void **arg)
{
	mdmat *md = Val_mdmat(arg[0]);
	int n = Val_int(arg[1]);

	if	(md && n * md->type->size < md->size)
	{
		Val_obj(rval) = LvalObj(&Lval_ref, md->type, md,
			(char *) md->data + n * md->type->size);
	}
	else	Val_obj(rval) = NULL;
}

static void MF_md2vec(EfiFunc *func, void *rval, void **arg)
{
	mdmat *md = Val_mdmat(arg[0]);

	Val_ptr(rval) = md ? NewEfiVec(md->type, md->data,
		md->size / md->type->size) : NULL;
}

static void MF_md2edb(EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = md2edb(Val_mdmat(arg[0]));
}

static void MF_show(EfiFunc *func, void *rval, void **arg)
{
	md_show(Val_io(arg[1]), Val_mdmat(arg[0]));
}

static void MF_clone(EfiFunc *func, void *rval, void **arg)
{
	Val_mdmat(rval) = md_clone(Val_mdmat(arg[0]));
}

extern void *md_DATA2(mdmat *md, unsigned mask, unsigned base, int lag);

extern void MF_valsum(EfiFunc *func, void *rval, void **arg);
extern void MF_leval(EfiFunc *func, void *rval, void **arg);
extern void MF_xeval(EfiFunc *func, void *rval, void **arg);

static void mdx_select (EfiFunc *func, void *rval, void **arg)
{
	mdaxis *x;
	MatchPar *mp;
	int n;
	
	x = Val_mdaxis(arg[0]);
	mp = MatchPar_create(Val_str(arg[1]), md_dim(x));

	for (n = 1; x; x = x->next, n++)
		if (MatchPar_exec(mp, StrPool_get(x->sbuf, x->i_name), n))
			break;

	rd_deref(mp);
	Val_mdaxis(rval) = x;
}

static void mdx_nselect (EfiFunc *func, void *rval, void **arg)
{
	mdaxis *x = Val_mdaxis(arg[0]);
	int n = Val_int(arg[1]);

	while (x && n > 0)
	{
		x = x->next;
		n--;
	}

	Val_mdaxis(rval) = x;
}

static void f_mdx_choice (EfiFunc *func, void *rval, void **arg)
{
	mdx_choice(Val_ptr(arg[0]), Val_str(arg[1]));
}

static void f_mdx_index (EfiFunc *func, void *rval, void **arg)
{
	mdx_index(Val_ptr(arg[0]), Val_int(arg[1]));
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_mdmat, "mdload (str file, str sel = NULL, str vsel = NULL)",
		MF_load },
	{ 0, &Type_bool, "mdfile (IO *io, str file, str mode = NULL)",
		MF_file },
	{ 0, &Type_mdmat, "mdread (IO io, str def = NULL)", MF_read },
	{ 0, &Type_mdmat, "reload (mdmat md, str sel = NULL, str vsel = NULL)",
		MF_reload },
	{ 0, &Type_mdmat, "mdmat::clone ()", MF_clone },
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
	{ FUNC_RESTRICTED, &Type_EDB, "mdmat ()", MF_md2edb },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (mdmat, int)", MF_index },
	{ FUNC_VIRTUAL, &Type_mdmat, "operator+ (mdmat, mdmat)", MF_term },
	{ FUNC_VIRTUAL, &Type_mdmat, "operator- (mdmat, mdmat)", MF_term },
	{ FUNC_VIRTUAL, &Type_mdmat, "operator* (mdmat, mdmat)", MF_term },
	{ FUNC_VIRTUAL, &Type_mdmat, "operator/ (mdmat, mdmat)", MF_term },

	{ FUNC_VIRTUAL, &Type_mdmat, "mdmat::expr (str op, mdmat)", MF_expr },
	{ FUNC_VIRTUAL, &Type_mdmat, "mdmat::expr (str op)", MF_expr },

	{ 0, &Type_bool, "mdmat::map (str path)", MF_map },
	{ 0, &Type_mdmat, "mdmat::save (str file, bool select = true)",
		MF_save },
	{ 0, &Type_mdmat, "mdmat::print (IO io = iostd, str def = NULL)",
		MF_print },
	{ 0, &Type_mdmat, "mdmat::konv (Type_t type)", MF_konv },
	{ 0, &Type_mdmat, "mdmat::round (double factor = 1.)", MF_round },
	{ 0, &Type_mdmat, "mdmat::adjust (double val = 0.)", MF_adjust },
	{ 0, &Type_mdmat, "mdmat::korr (bool round = false)", MF_korr },
	{ 0, &Type_mdmat, "mdmat::reduce (str list = NULL)", MF_reduce },
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
	{ 0, &Type_void, "mdmat::show (IO io = iostd)",
		MF_show },
	{ FUNC_VIRTUAL, &Type_mdaxis, "operator[] (mdaxis, str)",
		mdx_select },
	{ FUNC_VIRTUAL, &Type_mdaxis, "operator[] (mdaxis, int)",
		mdx_nselect },
	{ FUNC_VIRTUAL, &Type_void, "mdaxis::choice(str)", f_mdx_choice },
	{ FUNC_VIRTUAL, &Type_void, "mdaxis::choice(int)", f_mdx_index },
};


void MdSetup_func (void)
{
	AddFuncDef(fdef, tabsize(fdef));
}
