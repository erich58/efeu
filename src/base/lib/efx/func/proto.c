/*
Funktionsprototypen generieren

$Copyright (C) 1994 Erich Fr�hst�ck
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/object.h>
#include <EFEU/Op.h>
#include <ctype.h>


static int do_copy (io_t *io, io_t *tmp);

static Type_t *get_type(io_t *io, char *name)
{
	Type_t *type;

	if	((type = GetType(name)) == NULL)
		io_error(io, MSG_EFMAIN, 83, 1, name);

	return Parse_type(io, type);
}


/*	Funktionsprototyp aus String generieren
*/

Func_t *Prototype(const char *fmt, Type_t *type, Name_t *name, unsigned flags)
{
	io_t *io;
	Func_t *func;

	io = io_cstr(fmt);
	func = MakePrototype(io, type, name, flags);
	io_close(io);
	return func;
}


/*	Funktionsprototyp einlesen
*/

Func_t *MakePrototype(io_t *io, Type_t *type, Name_t *nptr, unsigned flags)
{
	char *name;
	Func_t *func;
	FuncEval_t eval;
	Type_t *otype;
	VarTab_t *vtab;
	FuncArg_t arg;
	io_t *tmp;
	int rflag;	/* R�ckgabe von L-Wert */
	int lflag;	/* L-Wert funktion */
	int weight;
	int virtual;
	int n;
	int c;

	lflag = 0;
	rflag = (flags & FUNC_LRETVAL) ? 1 : 0;
	virtual = (flags & FUNC_VIRTUAL) ? 1 : 0;

	if	(flags & FUNC_RESTRICTED)	weight = KONV_RESTRICTED;
	else if	(flags & FUNC_PROMOTION)	weight = KONV_PROMOTION;
	else					weight = KONV_STANDARD;

	eval = (flags & FUNC_INLINE) ? Func_inline : Func_func;

/*	Datentype bestimmen
*/
	while (type == NULL)
	{
		void *tname;

		if	((tname = io_getname(io)) == NULL)
		{
			io_error(io, MSG_EFMAIN, 82, 0);
			return NULL;
		}

		if	(strcmp("promotion", tname) == 0)
			weight = KONV_PROMOTION;
		else if	(strcmp("restricted", tname) == 0)
			weight = KONV_RESTRICTED;
		else if	(strcmp("virtual", tname) == 0)
			virtual = 1;
		else if	(strcmp("inline", tname) == 0)
			eval = Func_inline;
		else if	((type = get_type(io, tname)) == NULL)
			return NULL;

		memfree(tname);
	}

/*	Funktionsnamen bestimmen
*/
	if	(nptr == NULL)
	{
		c = io_eat(io, "%s");

		if	(c == '&')
		{
			rflag = 1;
			io_getc(io);
			c = io_eat(io, "%s");
		}
		
		nptr = Parse_sname(io, NULL);
	}

	if	(nptr == NULL)
	{
		io_error(io, MSG_EFMAIN, 81, 0);
		return NULL;
	}

	otype = NULL;
	vtab = NULL;
	name = NULL;
	nptr->obj = EvalObj(nptr->obj, NULL);

	if	(nptr->obj)
	{
		if	(nptr->obj->type == &Type_type)
		{
			otype = Val_type(nptr->obj->data);
			vtab = RefVarTab(otype->vtab);
		}
		else if	(nptr->obj->type == &Type_vtab)
		{
			vtab = RefVarTab(Val_vtab(nptr->obj->data));
		}
		else
		{
			io_error(io, MSG_EFMAIN, 225, 1, nptr->obj->type->name);
			UnrefObj(nptr->obj);
			memfree(nptr->name);
			return NULL;
		}

		UnrefObj(nptr->obj);
		nptr->obj = NULL;
	}
	else if	((otype = GetType(nptr->name)) != NULL)
	{
		memfree(nptr->name);
		nptr->name = NULL;
	}

	name = nptr->name;

/*	Flags bestimmen
*/
	c = io_eat(io, "%s");

	if	(c == '&')
	{
		lflag = 1;
		io_getc(io);
		c = io_eat(io, "%s");
	}

/*	Auf Argumentliste testen
*/
	if	(c != '(')
	{
		io_error(io, MSG_EFMAIN, 87, 1, name);
		DelVarTab(vtab);
		memfree(name);
		return NULL;
	}

	io_getc(io);
	c = io_eat(io, "%s");

/*	Konverter �berpr�fen
*/
	if	(name == NULL)
	{
		virtual = 1;

		if	(c == ')')	;
		else if	(otype == type)	otype = NULL; /* Konstruktor */
		else
		{
			io_error(io, MSG_EFMAIN, 223, 1, type->name);
			return NULL;
		}
	}

/*	Test auf void - Kennung
*/
	if	(c == 'v' && io_testkey(io, "void"))
	{
		if	(io_eat(io, "%s") != ')')
		{
			io_error(io, MSG_EFMAIN, 224, 1, type->name);
			return NULL;
		}
	}

/*	Funktionsargumente bestimmen
*/
	tmp = io_tmpbuf(0);
	n = 0;

	if	(otype)
	{
		arg.name = "this";
		arg.type = (otype == &Type_obj) ? NULL : otype;
		arg.lval = lflag;
		arg.nokonv = 1;
		arg.cnst = (arg.lval == 0);
		arg.defval = NULL;
		io_write(tmp, &arg, sizeof(FuncArg_t));
		n++;
	}

	while ((c = do_copy(io, tmp)) == FARG_STDARG)
		n++;

	if	(c == FARG_ERROR)
	{
		io_close(tmp);
		return NULL;
	}

/*	�berpr�fung der R�ckgabereferenz
*/
	if	(rflag && n == 0)
	{
		io_error(io, MSG_EFMAIN, 96, 1, name);
		io_close(tmp);
		return NULL;
	}

	func = NewFunc();
	func->type = (type == &Type_obj) ? NULL : type;
	func->lretval = rflag;
	func->scope = vtab;
	func->name = name;
	func->bound = (otype ? 1 : 0);
	func->virtual = virtual;
	func->weight = weight;
	func->eval = eval;

/*	Test auf variable Argumentzahl
*/
	if	(c == FARG_ELLIPSE)
	{
		func->vaarg = 1;
		arg.name = "va_list";
		arg.type = &Type_list;
		arg.lval = 0;
		arg.nokonv = 0;
		arg.cnst = 0;
		arg.defval = NULL;
		io_write(tmp, &arg, sizeof(FuncArg_t));
		n++;
	}

	func->dim = n;
	func->arg = ALLOC(n, FuncArg_t);
	io_rewind(tmp);
	io_read(tmp, func->arg, n * sizeof(FuncArg_t));
	io_close(tmp);

	if	(rflag && (func->type != func->arg[0].type))
	{
		message(NULL, MSG_EFMAIN, 97, 1, name);
	}

	return func;
}


/*	Argumentwert kopieren
*/

static int do_copy(io_t *io, io_t *tmp)
{
	FuncArg_t arg;
	int flag;

	flag = ParseFuncArg(io, &arg);

	if	(flag == FARG_STDARG)
		io_write(tmp, &arg, sizeof(FuncArg_t));

	return flag;
}


int ParseFuncArg(io_t *io, FuncArg_t *arg)
{
	void *p;
	int c;

	arg->name = NULL;
	arg->type = NULL;
	arg->defval = NULL;
	arg->lval = 0;
	arg->nokonv = 0;
	arg->cnst = 0;

	for (;;)
	{
		c = io_eat(io, "%s");

		if	(c == EOF || c == ')')
		{
			io_getc(io);
			return FARG_END;
		}
		else if	(c == '.')
		{
			p = io_mgets(io, "!.");
			c = io_eat(io, "%s");

			if	(strcmp(p, "...") == 0)
			{
				if	(c == EOF || c == ')')
				{
					io_getc(io);
					return FARG_ELLIPSE;
				}

				memfree(p);
				p = io_mgets(io, ")");
				io_error(io, MSG_EFMAIN, 84, 1, p);
				memfree(p);
				return FARG_ERROR;
			}
			else if	(strcmp(p, ".") != 0)
			{
				io_error(io, MSG_EFMAIN, 85, 1, p);
				memfree(p);
				return FARG_ERROR;
			}

			memfree(p);
			break;
		}
		else
		{
			if	((p = io_getname(io)) == NULL)
			{
				errmsg(MSG_EFMAIN, 89);
				return FARG_ERROR;
			}

			if	(mstrcmp("const", p) == 0)
			{
				memfree(p);
				p = NULL;
				arg->cnst = 1;
			}
			else
			{
				arg->type = get_type(io, p);
				memfree(p);

				if	(arg->type == NULL)
					return FARG_ERROR;

				break;
			}
		}
	}

	for (;;)
	{
		c = io_eat(io, "%s");

		if	(c == '&')	arg->lval = 1;
		else if	(c == '*')	arg->nokonv = 1;
		else			break;

		io_getc(io);
	}

	arg->name = io_mgets(io, "=,%s)");
	c = io_eat(io, "%s");

	if	(c == '=')
	{
		if	(arg->lval)
		{
			reg_cpy(1, arg->name);
			errmsg(MSG_EFMAIN, 86);
			return FARG_ERROR;
		}

		io_getc(io);
		arg->defval = Parse_term(io, OpPrior_Assign);
		c = io_eat(io, "%s");
	}

	if	(c == ',')
		io_getc(io);

	return FARG_STDARG;
}


/*	Funktionsargumente abfragen
*/

size_t GetFuncArg(io_t *io, FuncArg_t **arg)
{
	size_t n;
	int c;
	io_t *tmp;

	tmp = io_tmpbuf(0);
	n = 0;

	while ((c = do_copy(io, tmp)) == FARG_STDARG)
		n++;

	if	(c == FARG_ERROR)
	{
		io_close(tmp);
		return 0;
	}

	if	(c == FARG_ELLIPSE)
	{
		errmsg(MSG_EFMAIN, 146);
	}

	*arg = ALLOC(n, FuncArg_t);
	io_rewind(tmp);
	io_read(tmp, *arg, n * sizeof(FuncArg_t));
	io_close(tmp);
	return n;
}
