/*	Arbeiten mit multidimensionalen Matrizen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <EFEU/stdtype.h>

EfiType Type_mdmat = REF_TYPE("mdmat", mdmat *);
EfiType Type_mdaxis = SIMPLE_TYPE("mdaxis", mdaxis *, &Type_ptr, NULL);
EfiType Type_mdidx = SIMPLE_TYPE("mdidx", mdindex, NULL, NULL);

mdmat *Buf_mdmat = NULL;
mdaxis *Buf_mdaxis = NULL;
mdindex Buf_mdidx = { NULL, 0 };

static EfiObj *get_title (const EfiObj *base, void *data)
{
	mdmat *md = base ? Val_ptr(base->data) : NULL;
	return md ? LvalObj(&Lval_ref, &Type_str, md, &md->title) : NULL;
}

static EfiObj *get_type (const EfiObj *base, void *data)
{
	mdmat *md = base ? Val_ptr(base->data) : NULL;
	return md ? ConstObj(&Type_type, &md->type) : NULL;
}

static EfiObj *get_dim (const EfiObj *base, void *data)
{
	mdmat *md = base ? Val_ptr(base->data) : NULL;
	int dim = md ? md_dim(md->axis) : 0;
	return ConstObj(&Type_int, &dim);
}

static EfiObj *get_size (const EfiObj *base, void *data)
{
	mdmat *md = base ? Val_ptr(base->data) : NULL;
	int size = md ? md->size : 0;
	return ConstObj(&Type_int, &size);
}

static EfiObj *get_axis (const EfiObj *base, void *data)
{
	mdmat *md = base ? Val_ptr(base->data) : NULL;
	return md ? LvalObj(&Lval_ref, &Type_mdaxis, md, &md->axis) : NULL;
}

static EfiMember var_mdmat[] = {
	{ "title", &Type_str, get_title, NULL,
		":*:title of data cube\n"
		":de:Titel der Datenmatrix\n" },
	{ "type", &Type_type, get_type, NULL },
	{ "dim", &Type_int, get_dim, NULL },
	{ "size", &Type_int, get_size, NULL },
	{ "axis", &Type_mdaxis, get_axis, NULL },
};

static EfiObj *x_index (const EfiObj *base, void *data)
{
	mdaxis *x = base ? Val_mdaxis(base->data) : NULL;
	return x ? EfiVecObj(&Type_mdidx, x->idx, x->dim) : NULL;
}

static EfiObj *x_name (const EfiObj *base, void *data)
{
	if	(base)
	{
		mdaxis *x = Val_ptr(base->data);
		return LvalObj(&Lval_obj, &Type_str, base, &x->name);
	}
	else	return NULL;
}

static EfiObj *x_next (const EfiObj *base, void *data)
{
	if	(base)
	{
		mdaxis *x = Val_ptr(base->data);
		return x ? LvalObj(&Lval_obj, &Type_mdaxis, base, &x->next)
			: NULL;
	}
	else	return NULL;
}

static EfiObj *x_dim (const EfiObj *base, void *par)
{
	mdaxis *x = base ? Val_ptr(base->data) : NULL;
	int dim = x ? x->dim : 0;
	return ConstObj(&Type_int, &dim);
}

static EfiObj *x_flags (const EfiObj *base, void *par)
{
	mdaxis *x = base ? Val_ptr(base->data) : NULL;
	int flags = x ? x->flags : 0;
	return ConstObj(&Type_int, &flags);
}

static EfiMember var_mdaxis[] = {
	{ "name", &Type_str, x_name, NULL },
	{ "dim", &Type_int, x_dim, NULL },
	{ "flags", &Type_int, x_flags, NULL },
	{ "next", &Type_mdaxis, x_next, NULL },
	{ "index", &Type_vec, x_index, NULL },
};

static EfiObj *i_name (const EfiObj *base, void *par)
{
	if	(base)
	{
		mdindex *idx = base->data;
		return LvalObj(&Lval_obj, &Type_str, base, &idx->name);
	}
	else	return NULL;
}

static EfiObj *i_flags (const EfiObj *base, void *par)
{
	return int2Obj(base ? ((mdindex *) base->data)->flags : 0);
}


static EfiMember var_mdidx[] = {
	{ "name", &Type_str, i_name, NULL },
	{ "flags", &Type_int, i_flags, NULL },
};


/*	Ausgabefunktionen
*/

static void print_mdaxis (EfiFunc *func, void *rval, void **arg)
{
	int n;
	mdaxis *x;
	IO *io;

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

static void print_mdidx (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = io_printf(Val_io(arg[0]), "(%#s, %d)",
		Val_mdidx(arg[1]).name, Val_mdidx(arg[1]).flags);
}

/*
static void ptr_to_mdmat (EfiFunc *func, void *rval, void **arg)
{
	Val_mdmat(rval) = NULL;
}
*/

static void any_to_mdmat (EfiFunc *func, void *rval, void **arg)
{
	register EfiObj *obj = arg[0];
	Val_mdmat(rval) = obj && obj->type != &Type_ptr ?
		md_skalar(obj->type, obj->data) : NULL;
}

/*
static void str_to_mdmat (EfiFunc *func, void *rval, void **arg)
{
	Val_mdmat(rval) = md_skalar(&Type_str, arg[0]);
}
*/

static EfiFuncDef pdef[] = {
	{ FUNC_VIRTUAL, &Type_int, "fprint(IO, mdaxis)", print_mdaxis },
	{ FUNC_VIRTUAL, &Type_int, "fprint(IO, mdidx)", print_mdidx },
	/*
	{ 0, &Type_mdmat, "mdmat (_Ptr_)", ptr_to_mdmat },
	{ 0, &Type_mdmat, "mdmat (str)", str_to_mdmat },
	*/
	{ 0, &Type_mdmat, "mdmat (.)", any_to_mdmat },
};

static EfiVarDef vardef[] = {
	{ "MdClassListFormat", &Type_str, &MdClassListFormat,
		":*:parameter for displaying count classes\n"
		":de:Parameter zur Darstellung von Zählklassen\n" },
	{ "MdClassPrintHead", &Type_str, &MdClassPrintHead,
		":*:parameter for displaying count classes\n"
		":de:Parameter zur Darstellung von Zählklassen\n" },
	{ "MdClassPrintEntry", &Type_str, &MdClassPrintEntry,
		":*:parameter for displaying count classes\n"
		":de:Parameter zur Darstellung von Zählklassen\n" },
	{ "MdClassPrintFoot", &Type_str, &MdClassPrintFoot,
		":*:parameter for displaying count classes\n"
		":de:Parameter zur Darstellung von Zählklassen\n" },
	{ "MdClassPrintLimit", &Type_int, &MdClassPrintLimit,
		":*:parameter for displaying count classes\n"
		":de:Parameter zur Darstellung von Zählklassen\n" },
};

void SetupMdMat(void)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;

	SetupStd();
	SetupEDB();
	AddType(&Type_mdmat);
	AddType(&Type_mdaxis);
	AddType(&Type_mdidx);
	AddFuncDef(pdef, tabsize(pdef));
	AddVarDef(NULL, vardef, tabsize(vardef));
	AddEfiMember(Type_mdmat.vtab, var_mdmat, tabsize(var_mdmat));
	AddEfiMember(Type_mdaxis.vtab, var_mdaxis, tabsize(var_mdaxis));
	AddEfiMember(Type_mdidx.vtab, var_mdidx, tabsize(var_mdidx));
	MdSetup_count();
	MdSetup_func();
	Setup_edb2md();
}
