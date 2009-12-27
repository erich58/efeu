/*	Arbeiten mit multidimensionalen Matrizen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <EFEU/stdtype.h>

Type_t Type_mdmat = REF_TYPE("mdmat", mdmat_t *);
Type_t Type_mdaxis = SIMPLE_TYPE("mdaxis", mdaxis_t *, &Type_ptr);
Type_t Type_mdidx = SIMPLE_TYPE("mdidx", mdidx_t, NULL);

mdmat_t *Buf_mdmat = NULL;
mdaxis_t *Buf_mdaxis = NULL;
mdidx_t Buf_mdidx = { NULL, 0 };

static int buf_int = 0;

#define	MEM_FUNC(name, buf, expr)	\
static void *name (void *ptr) \
{ buf = (expr); return &buf; }

#define	P_MDMAT(a, d)	(Val_mdmat(ptr) ? Val_mdmat(ptr)->a : d)
#define	P_MDAXIS(a, d)	(Val_mdaxis(ptr) ? Val_mdaxis(ptr)->a : d)

static char **m_title(mdmat_t **ptr)
{
	return (*ptr != NULL) ? &(*ptr)->title : NULL;
}

static Type_t **m_type(mdmat_t **ptr)
{
	return (*ptr != NULL) ? &(*ptr)->type : NULL;
}

static int *m_size(mdmat_t **ptr)
{
	buf_int = (*ptr != NULL) ? (*ptr)->size : 0;
	return &buf_int;
}

static int *m_dim(mdmat_t **ptr)
{
	buf_int = (*ptr != NULL) ? md_dim((*ptr)->axis) : 0;
	return &buf_int;
}

static mdaxis_t **m_axis(mdmat_t **ptr)
{
	return (*ptr != NULL) ? &(*ptr)->axis : NULL;
}


static Obj_t *x_index (const Var_t *st, const Obj_t *obj)
{
	mdaxis_t *x;
	
	x = (obj ? Val_mdaxis(obj->data) : NULL);

	if	(x == NULL)	return NULL;

	Buf_vec.type = &Type_mdidx;
	Buf_vec.dim = x->dim;
	Buf_vec.data = x->idx;
	return NewObj(&Type_vec, &Buf_vec);
}

static char **x_name(mdaxis_t **ptr)
{
	/*
	Buf_mdaxis = Val_mdaxis(ptr);
	return Buf_mdaxis ? &Buf_mdaxis->name : NULL;
	*/
	return (*ptr != NULL) ? &(*ptr)->name : NULL;
}

MEM_FUNC(x_next, Buf_mdaxis, P_MDAXIS(next, NULL))
MEM_FUNC(x_dim, buf_int, P_MDAXIS(dim, 0))
MEM_FUNC(x_flags, buf_int, P_MDAXIS(flags, 0))

static char **i_name(mdidx_t *idx)
{
	return &idx->name;
}

static int *i_flags(mdidx_t *idx)
{
	return &idx->flags;
}


static Var_t var_mdmat[] = {
	{ "title", &Type_str, NULL, 0, 0, LvalMember, m_title },
	{ "type", &Type_type, NULL, 0, 0, ConstMember, m_type },
	{ "size", &Type_int, NULL, 0, 0, ConstMember, m_size },
	{ "dim", &Type_int, NULL, 0, 0, ConstMember, m_dim },
	{ "axis", &Type_mdaxis,	NULL, 0, 0, LvalMember,	m_axis },
};

static Var_t var_mdaxis[] = {
	{ "name", &Type_str, NULL, 0, 0, LvalMember, x_name },
	{ "dim", &Type_int, NULL, 0, 0, ConstMember, x_dim },
	{ "flags", &Type_int, NULL, 0, 0, ConstMember, x_flags },
	{ "next", &Type_mdaxis, NULL, 0, 0, LvalMember, x_next },
	{ "index", &Type_vec, NULL, 0, 0, x_index, NULL },
};

static Var_t var_mdidx[] = {
	{ "name", &Type_str, NULL, 0, 0, LvalMember, i_name },
	{ "flags", &Type_int, NULL, 0, 0, ConstMember, i_flags },
};


/*	Ausgabefunktionen
*/

static void print_mdaxis (Func_t *func, void *rval, void **arg)
{
	int n;
	mdaxis_t *x;
	io_t *io;

	x = Val_mdaxis(arg[1]);
	io = Val_io(arg[0]);

	if	(x != NULL)
	{
		n = io_printf(io, "(name = %#s, dim = %d, size = %d, flags = %b)",
			x->name, x->dim, x->size, x->flags);
	}
	else	n = io_puts("NULL", io);

	Val_int(rval) = n;
}

static void print_mdidx (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = io_printf(Val_io(arg[0]), "(%#s, %d)",
		Val_mdidx(arg[1]).name, Val_mdidx(arg[1]).flags);
}

/*
static void ptr_to_mdmat (Func_t *func, void *rval, void **arg)
{
	Val_mdmat(rval) = NULL;
}
*/

static void any_to_mdmat (Func_t *func, void *rval, void **arg)
{
	register Obj_t *obj = arg[0];
	Val_mdmat(rval) = obj && obj->type != &Type_ptr ?
		md_skalar(obj->type, obj->data) : NULL;
}

/*
static void str_to_mdmat (Func_t *func, void *rval, void **arg)
{
	Val_mdmat(rval) = md_skalar(&Type_str, arg[0]);
}
*/

static FuncDef_t pdef[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint(IO, mdaxis)", print_mdaxis },
	{ FUNC_VIRTUAL, &Type_int, "fprint(IO, mdidx)", print_mdidx },
	/*
	{ 0, &Type_mdmat, "mdmat (_Ptr_)", ptr_to_mdmat },
	{ 0, &Type_mdmat, "mdmat (str)", str_to_mdmat },
	*/
	{ 0, &Type_mdmat, "mdmat (.)", any_to_mdmat },
};

int IgnoreMdSizeError = 0;

static Var_t vardef[] = {
	{ "mddebug", &Type_bool, &md_reftype.debug },
	{ "IgnoreMdSizeError", &Type_bool, &IgnoreMdSizeError },
	{ "MdClassListFormat", &Type_str, &MdClassListFormat },
	{ "MdClassPrintHead", &Type_str, &MdClassPrintHead },
	{ "MdClassPrintEntry", &Type_str, &MdClassPrintEntry },
	{ "MdClassPrintFoot", &Type_str, &MdClassPrintFoot },
	{ "MdClassPrintLimit", &Type_int, &MdClassPrintLimit },
};

void SetupMdMat(void)
{
	AddType(&Type_mdmat);
	AddType(&Type_mdaxis);
	AddType(&Type_mdidx);
	AddFuncDef(pdef, tabsize(pdef));
	AddVar(NULL, vardef, tabsize(vardef));
	AddVar(Type_mdmat.vtab, var_mdmat, tabsize(var_mdmat));
	AddVar(Type_mdaxis.vtab, var_mdaxis, tabsize(var_mdaxis));
	AddVar(Type_mdidx.vtab, var_mdidx, tabsize(var_mdidx));
	MdFuncDef();
}
