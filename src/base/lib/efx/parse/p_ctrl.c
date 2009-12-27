/*	Kontrollfunktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/parsedef.h>

Obj_t *PFunc_for (io_t *io, void *data)
{
	Obj_t *a, *b, *c, *d;

	if	(!Parse_fmt(io, "(T", &a))
	{
		return NULL;
	}

	io_eat(io, " \t");

	if	(io_testkey(io, "in"))
	{
		ObjList_t *list;

		if	(a->type != &Type_name)
		{
			io_error(io, MSG_EFMAIN, 95, 0);
		}

		list = Parse_list(io, ')');

		if	(ObjListLen(list) == 1)
		{
			b = list->obj;
			list->obj = NULL;
			DelObjList(list);
		}
		else	b = Obj_list(list);

		if	(Parse_fmt(io, " C", &c))
			return Obj_call(Expr_forin, NULL, MakeObjList(3, a, b, c));

		UnrefObj(b);
	}
	else if	(Parse_fmt(io, "; T; T) C", &b, &c, &d))
	{
		return Obj_call(Expr_for, NULL, MakeObjList(4, a, b, c, d));
	}
	
	UnrefObj(a);
	return NULL;
}

Obj_t *PFunc_while (io_t *io, void *data)
{
	Obj_t *a, *b;

	if	(Parse_fmt(io, "(T) C", &a, &b))
	{
		return Obj_call(Expr_for, NULL, MakeObjList(4, NULL, a, NULL, b));
	}
	else	return NULL;
}

Obj_t *PFunc_do (io_t *io, void *data)
{
	Obj_t *a, *b;

	if	(Parse_fmt(io, "C while(T)", &a, &b))
	{
		return Obj_call(Expr_for, NULL,
			MakeObjList(4, a, b, NULL, RefObj(a)));
	}
	else	return NULL;
}

Obj_t *PFunc_if (io_t *io, void *data)
{
	Obj_t *a, *b, *c;

	if	(!Parse_fmt(io, "(T) C ", &a, &b))
		return NULL;

	if	(!io_testkey(io, "else"))
	{
		io_ungetc('\n', io);	/* !!! */
		c = NULL;
	}
	else	c = Parse_cmd(io);

	return Obj_call(Expr_void, NULL, NewObjList(CondTerm(a, b, c)));
}
