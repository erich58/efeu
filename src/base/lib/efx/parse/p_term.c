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

EfiObj *Parse_term(IO *io, int prior)
{
	EfiObj *left;
	EfiObj *a;

	if	((io_eat(io, " \t")) == EOF)
		return NULL;

	if	((left = Parse_obj(io, SCAN_ANYTYPE)) == NULL)
	{
		if	((left = Parse_op(io, 0, NULL)) == NULL)
			return NULL;
	}

	if	(left->type == &Type_type)
	{
		int flag = VDEF_REPEAT;

		if	(io_eat(io, " \t") == '&')
		{
			io_getc(io);
			flag |= VDEF_LVAL;
		}

		if	((a = Parse_vdef(io, Val_type(left->data), flag)))
		{
			UnrefObj(left);
			left = a;
		}
		else if	(flag & VDEF_LVAL)
		{
			left->type = &Type_lval;
		}
	}

	while ((a = Parse_op(io, prior, left)) != NULL)
		left = a;

	return left;
}


EfiObj *strterm(const char *str)
{
	IO *io;
	EfiObj *obj;

	io = io_cstr(str);
	obj = Parse_term(io, 0);
	io_close(io);
	return obj;
}


/*	Liste parsen
*/

EfiObjList *Parse_list(IO *io, int endchar)
{
	EfiObj *obj;
	EfiObjList *list;
	EfiObjList **ptr;
	char *prompt;
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
			io_error(io, "[efmain:181]", NULL);
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
				io_error(io, "[efmain:183]", "c", c);
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
				io_error(io, "[efmain:182]",
					"m", io_mgets(io, "%s"));
			}

			break;
		}
	}

	io_prompt(io, prompt);
	return list;
}
