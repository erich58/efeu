/*	Parser für Variablendefinitionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/stdtype.h>
#include <EFEU/Op.h>

static void d_vdef (const Type_t *type, VarDef_t *tg);
static void c_vdef (const Type_t *type, VarDef_t *tg, const VarDef_t *src);
static Obj_t *e_vdef (const Type_t *type, const VarDef_t *def);

Type_t Type_vdef = EVAL_TYPE("_VarDef_", VarDef_t,
	(Eval_t) e_vdef, (Clean_t) d_vdef, (Copy_t) c_vdef);

/*	Scope-Namen lesen
*/

Name_t *Parse_sname (io_t *io, Name_t *buf)
{
	int c;

	if	(buf == NULL)	buf = &Buf_name;

	buf->obj = NULL;
	buf->name = io_getname(io);

	if	(buf->name == NULL)	return NULL;

	c = io_eat(io, " \t");

	if	(c == ':')
	{
		c = io_getc(io);

		if	(io_peek(io) == ':')
		{
			Type_t *type;

			if	(buf->name == NULL)
			{
				buf->obj = NewPtrObj(&Type_vtab,
					RefVarTab(GlobalVar));
			}
			else if	((type = GetType(buf->name)) != NULL)
			{
				buf->obj = type2Obj(type);
				memfree(buf->name);
			}
			else if	(buf->obj == NULL)
			{
				buf->obj = NewPtrObj(&Type_name, buf->name);
			}
			else	buf->obj = NewObj(&Type_sname, buf);

			io_getc(io);
			buf->name = io_getname(io);
		}
		else	io_ungetc(c, io);
	}

	if	(buf->name && strcmp(buf->name, "operator") == 0)
	{
		memfree(buf->name);

		if	(io_eat(io, " \t") == '"')
		{
			io_scan(io, SCAN_STR, (void **) &buf->name);
		}
		else	buf->name = io_mgets(io, "%s");
	}

	if	(buf->name)	return buf;

	if	(buf->obj)
	{
		UnrefObj(buf->obj);
		io_error(io, MSG_EFMAIN, 81, 0);
	}

	return NULL;
}


/*	Index lesen
*/

ObjList_t *Parse_idx (io_t *io)
{
	ObjList_t *list;
	ObjList_t **ptr;
	int c;

	list = NULL;
	ptr = &list;

	while ((c = io_eat(io, " \t")) == '[')
	{
		io_getc(io);
		*ptr = NewObjList(Parse_index(io));
		ptr = &(*ptr)->next;
	}

	return list;
}


/*	Variablendefinition lesen
*/


Obj_t *Parse_vdef(io_t *io, Type_t *type, int flag)
{
	VarDef_t vdef;
	ObjList_t **ptr;
	Obj_t *obj;
	int c;

	c = io_eat(io, " \t");

	if	(c == '&')
		return Parse_func(io, type, NULL, 0);

/*	Variablenname bestimmen
*/
	if	(Parse_sname(io, &vdef.name) == NULL)
		return NULL;

	c = io_eat(io, " \t");

	if	(c == '&' || c == '(')
		return Parse_func(io, type, &vdef.name, 0);

/*	Indexliste generieren
*/
	vdef.type = type;
	vdef.idx = NULL;
	ptr = &vdef.idx;
	
	while (c == '[')
	{
		io_getc(io);
		*ptr = NewObjList(Parse_index(io));
		ptr = &(*ptr)->next;
		c = io_eat(io, " \t");
	}

/*	Initialisierer bestimmen
*/
	if	(c == '=')
	{
		io_getc(io);
		vdef.defval = Parse_term(io, OpPrior_Assign);
		c = io_eat(io, " \t");
	}
	else	vdef.defval = NULL;

	obj = NewObj(&Type_vdef, &vdef);

/*	Bei Flag: Weitere Variablen zum gleichen Type bestimmen
*/
	if	(flag && c == ',')
	{
		io_getc(io);
		obj = CommaTerm(obj, Parse_vdef(io, type, flag));
	}

	return obj;
}


ObjList_t *VarDefList(io_t *io, int delim)
{
	ObjList_t *list, **ptr;
	Type_t *type;
	Obj_t *obj;
	int err;
	int c;

	list = NULL;
	ptr = &list;
	err = 0;

	while (err == 0 && (c = io_eat(io, "%s;")) != delim)
	{
		if	(c == EOF)
		{
			err = 121;
			break;
		}

		if	((obj = Parse_obj(io, SCAN_NAME)) == NULL)
		{
			err = 122;
			break;
		}

		if	(obj->type != &Type_type)
		{
			err = 122;
			UnrefObj(obj);
			break;
		}

		type = Val_type(obj->data);
		UnrefObj(obj);

		for (;;)
		{
			obj = Parse_vdef(io, type, 0);

			if	(obj == NULL)
			{
				err = 122;
				break;
			}

			*ptr = NewObjList(obj);
			ptr = &(*ptr)->next;
			c = io_eat(io, " \t");

			if	(c == ',')
			{
				io_getc(io);
			}
			else	break;
		}
	}

	if	(err)
	{
		io_error(io, MSG_EFMAIN, err, 0);
		DelObjList(list);
		return NULL;
	}

	return list;
}


static void d_vdef(const Type_t *type, VarDef_t *tg)
{
	UnrefObj(tg->name.obj);
	memfree(tg->name.name);
	DelObjList(tg->idx);
	UnrefObj(tg->defval);
	memset(tg, 0, sizeof(VarDef_t));
}


static void c_vdef(const Type_t *type, VarDef_t *tg, const VarDef_t *src)
{
	tg->type = src->type;
	tg->name.obj = RefObj(src->name.obj);
	tg->name.name = mstrcpy(src->name.name);
	tg->idx = RefObjList(src->idx);
	tg->defval = RefObj(src->defval);
}


/*	Variablendefinition auswerten
*/

static Obj_t *e_vdef(const Type_t *type, const VarDef_t *vdef)
{
	Var_t *var;
	Obj_t *x, *defval;
	Type_t *vartype;
	VarTab_t *vtab;
	size_t dim;

	vartype = vdef->type;
	defval = EvalObj(RefObj(vdef->defval), NULL);

	if	(vdef->idx)
	{
		defval = EvalObj(defval, &Type_list);
		vartype = VecType(vartype, vdef->idx->next);
		x = EvalObj(RefObj(vdef->idx->obj), &Type_int);

		if	(x)	
		{
			dim = Val_int(x->data);
			UnrefObj(x);
		}
		else if	(defval)
		{
			dim = ObjListLen(Val_list(defval->data));
		}
		else	dim = 0;
		
		if	(dim == 0)
		{
			errmsg(MSG_EFMAIN, 302);
			return NULL;
		}
	}
	else	dim = 0;

	vtab = Obj2Ptr(RefObj(vdef->name.obj), &Type_vtab);
	var = NewVar(vartype, vdef->name.name, dim);
	AddVar(vtab, var, 1);
	x = Var2Obj(var, NULL);

	if	(defval)
		return AssignObj(x, defval);

	return x;
}
