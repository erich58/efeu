/*
Funktionsprototypen generieren

$Copyright (C) 1994 Erich Frühstück
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
#include <EFEU/parsedef.h>
#include <EFEU/Op.h>
#include <ctype.h>


static int do_copy (IO *io, IO *tmp, int delim);

static EfiType *get_type (IO *io, char *name)
{
	EfiType *type = NULL;
	EfiParseDef *parse;

	if	((parse = GetParseDef(name)) != NULL)
	{
		EfiObj *obj = (*parse->func)(io, parse->data);

		if	(obj && obj->type == &Type_type)
			type = Val_type(obj->data);

		UnrefObj(obj);
	}
	else	type = XGetType(name);

	if	(type)
		return Parse_type(io, type);

	io_error(io, "[efmain:83]", "s", name);
	return NULL;
}


/*	Funktionsprototyp aus String generieren
*/

EfiFunc *Prototype(const char *fmt, EfiType *type, EfiName *name, unsigned flags)
{
	IO *io;
	EfiFunc *func;

	io = io_cstr(fmt);
	func = MakePrototype(io, type, name, flags);
	io_close(io);
	return func;
}


/*	Funktionsprototyp einlesen
*/

EfiFunc *MakePrototype(IO *io, EfiType *type, EfiName *nptr, unsigned flags)
{
	char *name;
	EfiFunc *func;
	void (*eval) (EfiFunc *func, void *rval, void **arg);
	EfiType *otype;
	EfiVarTab *vtab;
	EfiFuncArg arg;
	IO *tmp;
	int rflag;	/* Rückgabe von L-Wert */
	int lflag;	/* L-Wert funktion */
	int weight;
	int virfunc;
	int n;
	int c;

	lflag = 0;
	rflag = (flags & FUNC_LRETVAL) ? 1 : 0;
	virfunc = (flags & FUNC_VIRTUAL) ? 1 : 0;

	if	(flags & FUNC_RESTRICTED)	weight = CONV_RESTRICTED;
	else if	(flags & FUNC_PROMOTION)	weight = CONV_PROMOTION;
	else					weight = CONV_STANDARD;

	eval = (flags & FUNC_INLINE) ? Func_inline : Func_func;

/*	Datentype bestimmen
*/
	while (type == NULL)
	{
		void *tname;

		if	((tname = io_getname(io)) == NULL)
		{
			io_error(io, "[efmain:82]", NULL);
			return NULL;
		}

		if	(strcmp("promotion", tname) == 0)
			weight = CONV_PROMOTION;
		else if	(strcmp("restricted", tname) == 0)
			weight = CONV_RESTRICTED;
		else if	(strcmp("virtual", tname) == 0)
			virfunc = 1;
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
		io_error(io, "[efmain:81]", NULL);
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
			io_error(io, "[efmain:225]", "s",
				nptr->obj->type->name);
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
		io_error(io, "[efmain:87]", "s", name);
		DelVarTab(vtab);
		memfree(name);
		return NULL;
	}

	io_getc(io);
	c = io_eat(io, "%s");

/*	Konverter überprüfen
*/
	if	(name == NULL)
	{
		virfunc = 1;

		if	(c == ')')	;
		else if	(otype == type)	otype = NULL; /* Konstruktor */
		else
		{
			io_error(io, "[efmain:223]", "s", type->name);
			return NULL;
		}
	}

/*	Test auf void - Kennung
*/
	if	(c == 'v' && io_testkey(io, "void"))
	{
		if	(io_eat(io, "%s") != ')')
		{
			io_error(io, "[efmain:224]", "s", type->name);
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
		arg.desc = NULL;
		arg.type = (otype == &Type_obj) ? NULL : otype;
		arg.lval = lflag;
		arg.noconv = 1;
		arg.promote = 0;
		arg.cnst = (arg.lval == 0);
		arg.defval = NULL;
		io_write(tmp, &arg, sizeof arg);
		n++;
	}

	while ((c = do_copy(io, tmp, ')')) == FARG_STDARG)
		n++;

	if	(c == FARG_ERROR)
	{
		io_close(tmp);
		return NULL;
	}

/*	Überprüfung der Rückgabereferenz
*/
	if	(rflag && n == 0)
	{
		io_error(io, "[efmain:96]", "s", name);
		io_close(tmp);
		return NULL;
	}

	func = NewFunc(type == &Type_obj ? NULL : type, name);
	func->lretval = rflag;
	func->scope = vtab;
	func->bound = (otype ? 1 : 0);
	func->virfunc = virfunc;
	func->weight = weight;
	func->eval = eval;

/*	Test auf variable Argumentzahl
*/
	if	(c == FARG_ELLIPSE)
	{
		func->vaarg = 1;
		arg.name = "va_list";
		arg.desc = NULL;
		arg.type = &Type_list;
		arg.lval = 0;
		arg.noconv = 0;
		arg.promote = 0;
		arg.cnst = 0;
		arg.defval = NULL;
		io_write(tmp, &arg, sizeof arg);
		n++;
	}

	func->dim = n;
	func->arg = memalloc(n * sizeof func->arg[0]);
	io_rewind(tmp);
	io_read(tmp, func->arg, n * sizeof func->arg[0]);
	io_close(tmp);

	if	(rflag && (func->type != func->arg[0].type))
	{
		log_note(NULL, "[efmain:97]", "s", name);
	}

	return func;
}


/*	Argumentwert kopieren
*/

static int do_copy(IO *io, IO *tmp, int delim)
{
	EfiFuncArg arg;
	int flag;

	flag = ParseFuncArg(io, &arg, delim);

	if	(flag == FARG_STDARG)
		io_write(tmp, &arg, sizeof arg);

	return flag;
}


int ParseFuncArg(IO *io, EfiFuncArg *arg, int delim)
{
	void *p;
	int c;

	arg->name = NULL;
	arg->desc = NULL;
	arg->type = NULL;
	arg->defval = NULL;
	arg->lval = 0;
	arg->noconv = 0;
	arg->cnst = 0;
	arg->promote = 0;

	for (;;)
	{
		c = io_eat(io, "%s");

		if	(c == EOF || c == delim)
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
				if	(c == EOF || c == delim)
				{
					io_getc(io);
					return FARG_ELLIPSE;
				}

				memfree(p);
				p = io_mgets(io, ")");
				io_error(io, "[efmain:84]", "m", p);
				return FARG_ERROR;
			}
			else if	(strcmp(p, ".") != 0)
			{
				io_error(io, "[efmain:85]", "m", p);
				return FARG_ERROR;
			}

			memfree(p);
			break;
		}
		else
		{
			if	((p = io_getname(io)) == NULL)
			{
				log_note(NULL, "[efmain:89]", NULL);
				return FARG_ERROR;
			}

			if	(mstrcmp("const", p) == 0)
			{
				memfree(p);
				p = NULL;
				arg->cnst = 1;
			}
			else if (mstrcmp("restricted", p) == 0)
			{
				memfree(p);
				p = NULL;
				arg->noconv = 1;
			}
			else if (mstrcmp("promotion", p) == 0)
			{
				memfree(p);
				p = NULL;
				arg->promote = 1;
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
#if	1
		else if	(c == '*')	arg->noconv = 1;
#else
		else if	(c == '*')	abort();
#endif
		else			break;

		io_getc(io);
	}

	arg->name = Parse_name(io, 0);
	c = io_eat(io, "%s");

	if	(c == '=')
	{
		if	(arg->lval)
		{
			log_note(NULL, "[efmain:86]", "s", arg->name);
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

size_t GetFuncArg(IO *io, EfiFuncArg **arg, int delim)
{
	size_t n;
	int c;
	IO *tmp;

	tmp = io_tmpbuf(0);
	n = 0;

	while ((c = do_copy(io, tmp, delim)) == FARG_STDARG)
		n++;

	if	(c == FARG_ERROR)
	{
		io_close(tmp);
		return 0;
	}

	if	(c == FARG_ELLIPSE)
		log_note(NULL, "[efmain:146]", NULL);

	*arg = memalloc(n * sizeof **arg);
	io_rewind(tmp);
	io_read(tmp, *arg, n * sizeof **arg);
	io_close(tmp);
	return n;
}
