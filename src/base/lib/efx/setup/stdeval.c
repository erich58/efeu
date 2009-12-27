/*	Initialisierung des Befehlsinterpreters
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdeval.h>
#include <EFEU/ioctrl.h>


/*	Schlüsselwörter
*/

static Obj_t *pf_operator (io_t *io, void *data)
{
	void *p;

	if	(io_eat(io, " \t") == '"')
	{
		io_scan(io, SCAN_STR, &p);
	}
	else	p = io_mgets(io, "%s");

	return NewPtrObj(&Type_name, p);
}

static Obj_t *pf_func (io_t *io, void *data)
{
	return Parse_func(io, NULL, NULL, (int) (size_t) data);
}

static Obj_t *pf_expression (io_t *io, void *data)
{
	return expr2Obj(Parse_cmd(io));
}

static Obj_t *pf_string (io_t *io, void *data)
{
	return str2Obj(getstring(io));
}

static Obj_t *do_psub(void *par, const ObjList_t *list)
{
	return str2Obj(parsub(Val_str(list->obj->data)));
}

static Obj_t *pf_parsub (io_t *io, void *data)
{
	return Obj_call(do_psub, NULL, NewObjList(str2Obj(getstring(io))));
}

static Obj_t *pf_const (io_t *io, void *data)
{
	return EvalObj(Parse_term(io, 0), NULL);
}

static Obj_t *pf_static (io_t *io, void *data)
{
	UnrefEval(Parse_term(io, 0)); return Obj_noop();
}


static ParseDef_t pdef[] = {
	{ "for", PFunc_for, NULL },
	{ "while", PFunc_while, NULL },
	{ "do", PFunc_do, NULL },
	{ "if", PFunc_if, NULL },

	{ "operator", pf_operator, NULL },
	{ "function", pf_func, NULL },
	{ "virtual", pf_func, (void *) FUNC_VIRTUAL },
	{ "promotion", pf_func, (void *) FUNC_PROMOTION },
	{ "restricted", pf_func, (void *) FUNC_RESTRICTED },
	{ "inline", pf_func, (void *) FUNC_INLINE },

	{ "return", PFunc_return, NULL },
	{ "break", PFunc_break, (void *) 0 },
	{ "continue", PFunc_break, (void *) 1 },

	{ "expression", pf_expression, NULL },
	{ "string", pf_string, NULL },
	{ "parsub", pf_parsub, NULL },
	{ "static", pf_static, NULL },
	{ "const", pf_const, NULL },

	{ "struct", PFunc_struct, NULL },
	{ "typedef", PFunc_typedef, NULL },
};


/*	Komponenten von Listen
*/

static Obj_t *list_obj (const Var_t *st, const Obj_t *obj)
{
	register ObjList_t *list = Val_list(obj->data);
	return list ? RefObj(list->obj) : ptr2Obj(NULL);
}

static Obj_t *list_next (const Var_t *st, const Obj_t *obj)
{
	register ObjList_t *list = Val_list(obj->data);

	list = list ? RefObjList(list->next) : NULL;
	return NewPtrObj(&Type_list, list);
}


static Var_t list_member[] = {
	{ "next", &Type_list, NULL, 0, 0, list_next, NULL },
	{ "obj", NULL, NULL, 0, 0, list_obj, NULL },
};

/*	Komponenten von Typen
*/

static Obj_t *type_default (const Var_t *st, const Obj_t *obj)
{
	Type_t *type = Val_type(obj->data);

	if	(type == NULL)	return NULL;
	else if	(type->defval)	return LvalObj(type, NULL, type->defval);
	else			return NewObj(type, NULL);
}

static Var_t type_member[] = {
	{ "default", NULL, NULL, 0, 0, type_default, NULL },
};

/*	Komponenten von Referenzen
*/

static Obj_t *ref_debug (const Var_t *st, const Obj_t *obj)
{
	refdata_t *rd = Val_ptr(obj->data);
	return rd ? LvalObj(&Type_bool, NULL,
		(int *) &rd->reftype->debug) : NULL;
}

static Obj_t *ref_refcount (const Var_t *st, const Obj_t *obj)
{
	refdata_t *rd = Val_ptr(obj->data);
	return int2Obj(rd ? rd->refcount : 0);
}

static Obj_t *ref_ident (const Var_t *st, const Obj_t *obj)
{
	return str2Obj(rd_ident(Val_ptr(obj->data)));
}

static Var_t ref_member[] = {
	{ "ident", &Type_str, NULL, 0, 0, ref_ident, NULL },
	{ "debug", &Type_bool, NULL, 0, 0, ref_debug, NULL },
	{ "refcount", &Type_int, NULL, 0, 0, ref_refcount, NULL },
};

/*	Parametersubstitution
*/

static int c_term(io_t *in, io_t *out, void *arg)
{
	return iocpy_term(in, out, '(', ")", 0);
}

static int c_eval(io_t *in, io_t *out, void *arg)
{
	return iocpy_eval(in, out, 0, "}", 0);
}

#if	0
static int c_repeat(io_t *in, io_t *out, void *arg)
{
	int n, count;
	char *str;

	count = Obj2int(Parse_term(in, 0));

	do	n = io_getc(in);
	while	(n != ':' && n != EOF);

	str = io_mgets(in, arg);
	io_getc(in);
	n = 0;

	while (count-- > 0)
		n += io_psub(out, str);

	memfree(str);
	return n;
}
#endif

void SetupStd(void)
{
	GlobalVar = VarTab("global", 64);
	LocalVar = RefVarTab(GlobalVar);

	StdOpDef();

	AddParseDef(pdef, tabsize(pdef));
	CmdEval_setup();

	CmdSetup_const();
	CmdSetup_base();
	CmdSetup_int();
	CmdSetup_long();
	CmdSetup_double();
	CmdSetup_str();
	CmdSetup_io();

	AddType(&Type_name);
	AddType(&Type_mname);
	AddType(&Type_sname);
	AddType(&Type_undef);
	AddType(&Type_vec);
	AddType(&Type_ofunc);
	AddType(&Type_list);
	AddType(&Type_explist);
	AddType(&Type_vdef);

	CmdSetup_obj();
	CmdSetup_reg();
	CmdSetup_pctrl();
	CmdSetup_test();

	CmdSetup_konv();
	CmdSetup_print();
	CmdSetup_func();
	CmdSetup_op();
	CmdSetup_info();
	CmdSetup_strbuf();
	CmdSetup_date();
	CmdSetup_unix();

	AddVar(Type_list.vtab, list_member, tabsize(list_member));
	AddVar(Type_type.vtab, type_member, tabsize(type_member));
	AddVar(Type_ref.vtab, ref_member, tabsize(ref_member));

	psubfunc('(', c_term, ")");
	psubfunc('{', c_eval, "}");
/*
	psubfunc('<', c_repeat, ">");
*/
}
