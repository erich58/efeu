/*	Funktionsdefinition parsen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <ctype.h>


/*	Prototype - Generierung
*/

static Obj_t *get_func(Func_t *func)
{
	Func_t *f;
	VirFunc_t *vf;

	if	(func->bound)
		vf = GetTypeFunc(func->arg[0].type, func->name);
	else	vf = GetGlobalFunc(func->name);

	f = XGetFunc(func->type, vf, func->arg, func->dim);

	if	(f && f->refcount == 1)
		f->virtual = func->virtual;

	rd_deref(func);
	return NewObj(&Type_func, &f);
}


Obj_t *Parse_func(io_t *io, Type_t *type, Name_t *name, int flags)
{
	Func_t *func;
	char *prompt;
	int c;

	func = MakePrototype(io, type, name, flags);

	if	(func)
	{
		prompt = io_prompt(io, "function >>> ");
		c = io_eat(io, "%s");
		io_prompt(io, prompt);

		if	(c == ';' || c == ',' || c == '(' || c == ')')
		{
			if (c == ';') io_getc(io);

			return get_func(func);
		}
		else if	(c == '{')
		{
			io_getc(io);
			func->par = Parse_block(io, '}');
		}
		else
		{
			func->par = Parse_term(io, 0);

			if	(io_eat(io, " \t") == ';')
				io_getc(io);

			func->eval = Func_inline;
		}

		func->clean = (clean_t) UnrefObj;
		AddFunc(func);
	}

	return Obj_noop();
}
