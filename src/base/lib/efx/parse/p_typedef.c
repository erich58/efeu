/*	Typedefinition
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/Op.h>
#include <EFEU/parsedef.h>

static Var_t *get_struct (io_t *io, Type_t *type);


/*	Typedefinition: Falls kein Vektortype angegeben ist, wird nur
	ein Aliasname generiert.
*/

static void DummyKonv (Func_t *func, void *rval, void **arg)
{
	CopyData(func->type, rval, arg[0]);
}

extern void CopyKonv(Type_t *type, Type_t *base);

Obj_t *PFunc_typedef(io_t *io, void *arg)
{
	Type_t *type;
	Var_t *st;
	char *def;

	if	((st = get_struct(io, NULL)) == NULL)
		return NULL;

	type = NewType(st->name);
	st->name = NULL;
	type->base = st->dim ? NewVecType(st->type, st->dim) : st->type;
	type->copy = type->base->copy;
	type->clean = type->base->clean;
	type->size = type->base->size;
	type->recl = type->base->recl;
	type->iodata = type->base->iodata;
	type->defval = st->data;
	st->data = NULL;
	DelVar(st);

	CopyKonv(type, type->base);
	AddType(type);
	def = msprintf("%s ()", type->name);
	SetFunc(FUNC_PROMOTION, type->base, def, DummyKonv);
	memfree(def);
	return type2Obj(type);
}

#if	0
void AliasType(char *name, Type_t *type)
{
	Type_t *ntype;
	char *def;

	ntype = NewType(name);
	ntype->copy = type->copy;
	ntype->clean = type->clean;
	ntype->size = type->size;
	ntype->base = type;
	AddType(ntype);

	/*
	def = msprintf("%s ()", type->name);
	SetFunc(FUNC_PROMOTION, ntype, def, DummyKonv);
	*/
	def = msprintf("%s ()", ntype->name);
	SetFunc(FUNC_PROMOTION, (Type_t *) type, def, DummyKonv);
	memfree(def);
}
#endif



/*	Strukturkomponente bestimmen
*/

static Var_t *get_struct(io_t *io, Type_t *type)
{
	Var_t *st;
	Obj_t *obj;
	void *name;
	size_t dim;
	int n;
	int c;

	if	((type = Parse_type(io, type)) == NULL)
		return NULL;

	if	((name = io_getname(io)) == NULL)
	{
		io_error(io, MSG_EFMAIN, 123, 0);
		return NULL;
	}

	dim = 0;

	while ((c = io_eat(io, " \t")) == '[')
	{
		io_getc(io);
		obj = EvalObj(Parse_index(io), &Type_int);
		n = obj ? Val_int(obj->data) : 0;
		UnrefObj(obj);

		if	(n <= 0)
		{
			io_error(io, MSG_EFMAIN, 124, 1, name);
			memfree(name);
			return NULL;
		}
		else if	(dim)
		{
			type = NewVecType(type, n);
		}
		else	dim = n;
	}

	st = NewVar(type, name, dim);
	st->member = StructMember;
	st->offset = 0;

	if	(c == '=')
	{
		io_getc(io);
		Obj2Data(Parse_term(io, OpPrior_Assign), st->type, st->data);
	}

	return st;
}


/*	Strukturliste generieren
*/

Var_t *GetStruct (io_t *io, int delim)
{
	Type_t *type;
	Var_t *st, **ptr;
	int c;

	type = NULL;
	st = NULL;
	ptr = &st;

	while ((c = io_eat(io, " \t")) != delim)
	{
		switch (c)
		{
		case EOF:

			DelVar(st);
			io_error(io, MSG_EFMAIN, 121, 0);
			return NULL;

		case '\n':
		case ';':	type = NULL; /* FALLTHROUGH */
		case ',':	io_getc(io); continue;
		default:	break;
		}

		if	((*ptr = get_struct(io, type)) == NULL)
		{
			DelVar(st);
			return NULL;
		}

		type = (*ptr)->type;
		ptr = &(*ptr)->next;
	}

	return st;
}
