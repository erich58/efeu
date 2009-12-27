/*
Parser für Terme und Listen

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
#include <EFEU/Op.h>
#include <EFEU/printobj.h>

#define	PROMPT	", >>> "

/*	Term bestimmen
*/

Obj_t *Parse_term(io_t *io, int prior)
{
	Obj_t *left;
	Obj_t *a;

	if	((io_eat(io, " \t")) == EOF)
		return NULL;

	if	((left = Parse_obj(io, SCAN_ANYTYPE)) == NULL)
	{
		if	((left = Parse_op(io, 0, NULL)) == NULL)
			return NULL;
	}

	if	(left->type == &Type_type)
	{
		int flag = 0x1;

		if	(io_eat(io, " \t") == '&')
		{
			io_getc(io);
			flag |= 0x2;
		}

		if	((a = Parse_vdef(io, Val_type(left->data), flag)))
		{
			UnrefObj(left);
			left = a;
		}
		else if	(flag & 0x2)
		{
			left->type = &Type_lval;
		}
	}

	while ((a = Parse_op(io, prior, left)) != NULL)
		left = a;

	return left;
}


Obj_t *strterm(const char *str)
{
	io_t *io;
	Obj_t *obj;

	io = io_cstr(str);
	obj = Parse_term(io, 0);
	io_close(io);
	return obj;
}


/*	Liste parsen
*/

ObjList_t *Parse_list(io_t *io, int endchar)
{
	Obj_t *obj;
	ObjList_t *list;
	ObjList_t **ptr;
	char *prompt;
	char *p;
	char *eatdef;
	int flag;
	int c;

	list = NULL;
	ptr = &list;
	prompt = io_prompt(io, PROMPT);
	flag = 0;
	io_eat(io, "%s");
	eatdef = (endchar != EOF) ? "%s" : " \t";

	for (;;)
	{
		c = io_eat(io, eatdef);

		if	(c == endchar)
		{
			io_getc(io);
			break;
		}
		else if	(c == EOF)
		{
			io_error(io, MSG_EFMAIN, 181, 0);
			break;
		}
		else if	(flag)
		{
			if	((c = io_getc(io)) == ',')
			{
				io_eat(io, "%s");
				flag = 0;
			}
			else if	(endchar == EOF)
			{
				io_ungetc(c, io);
				break;
			}
			else
			{
				p = msprintf("%#c", c);
				io_error(io, MSG_EFMAIN, 183, 1, p);
				memfree(p);
				break;
			}
		}
		else if	((obj = Parse_term(io, OpPrior_Comma)))
		{
			*ptr = NewObjList(obj);
			ptr = &(*ptr)->next;
			flag = 1;
		}
		else
		{
			if	(endchar != EOF)
			{
				p = io_mgets(io, "%s");
				io_error(io, MSG_EFMAIN, 182, 1, p);
				memfree(p);
			}

			break;
		}
	}

	io_prompt(io, prompt);
	return list;
}


int iocpy_term(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	Obj_t *obj;
	ObjList_t *list;
	char *fmt;

	obj = EvalObj(Parse_term(in, OpPrior_Cond), NULL);
	c = io_getc(in);

	fmt = (c == ':') ? io_xgets(in, arg) : NULL;

	if	(!listcmp(arg, c))
		iocpy_skip(in, NULL, c, arg, 0);

	if	(obj && fmt)
	{
		list = NewObjList(obj);
		c = PrintFmtList(out, fmt, list);
		DelObjList(list);
	}
	else if	(obj)
	{
		c = PrintObj(out, obj);
		UnrefObj(obj);
	}
	else	c = 0;

	memfree(fmt);
	return c;
}
