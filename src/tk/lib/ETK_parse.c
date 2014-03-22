/*
:*:	parse tcl command
:de:	Tcl-Kommando einlesen

$Copyright (C) 2002 Erich Frühstück
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

#include <GUI/ETK.h>
#include <EFEU/stdtype.h>
#include <EFEU/parsedef.h>
#include <ctype.h>

#define	PROMPT	"tcl% "

static STRBUF(tcl_buf, 1024);

static void tcl_grp (IO *io, int beg, int end)
{
	int c;
	int quote;

	if	(beg)
		sb_putc(beg, &tcl_buf);

	quote = 0;

	while ((c = io_mgetc(io, 0)) != EOF)
	{
		if	(c == '\\')
		{
			sb_putc(c, &tcl_buf);
			c = io_mgetc(io, 0);

			if	(c == EOF)
				break;

			sb_putc(c, &tcl_buf);
		}
		else if	(c == end)
		{
			if	(c == '\n')
				io_ungetc(c, io);
			else if	(beg)
				sb_putc(c, &tcl_buf);

			break;
		}
		else if	(quote)
		{
			if	(c == quote)	quote = 0;

			sb_putc(c, &tcl_buf);
		}
		else
		{
			switch (c)
			{
			case '{':	tcl_grp(io, c, '}'); break;
			case '[':	tcl_grp(io, c, ']'); break;
			case '(':	tcl_grp(io, c, ')'); break;
			case '"':	quote = c; /* FALLTHROUGH */
			default:	sb_putc(c, &tcl_buf); break;
			}
		}
	}
}

static void tcl_name (IO *io)
{
	int c;
	
	do	c = io_mgetc(io, 0);
	while	(c == ' ' || c == '\t');

	while (isalnum(c) || c == '_')
	{
		sb_putc(c, &tcl_buf);
		c = io_mgetc(io, 0);
	}

	while (c == ' ' || c == '\t')
		c = io_mgetc(io, 0);

	switch (c)
	{
	case EOF:	break;
	case '(':	tcl_grp(io, c, ')'); break;
	default:	io_ungetc(c, io); break;
	}
}

static EfiObj *expr_var (void *par, const EfiObjList *list)
{
	return ETK_var(par, Val_str(list->obj->data));
}

static EfiObj *expr_expr (void *par, const EfiObjList *list)
{
#if	HAS_TCL
	ETK_eval(par, Val_str(list->obj->data));
	return ETK_result(par);
#else
	return RefObj(list->obj);
#endif
}

static EfiObj *expr_eval (void *par, const EfiObjList *list)
{
	ETK_eval(par, Val_str(list->obj->data));
	return NULL;
}

/*
:*:The function |$1| parses a tcl command from <io> and returns a
object of Type |_Expr_|.
:*:Die Funktion |$1| liest ein Tcl-Kommando aus der IO-Struktur <io> und
liefert ein Objekt vom Type |_Expr_|.
*/

EfiObj *ETK_parse (ETK *etk, IO *io)
{
	int c;
	char *prompt;
	EfiObj *(*expr) (void *par, const EfiObjList *list);

	if	(!io)	return NULL;

	sb_begin(&tcl_buf);
	prompt = io_prompt(io, PROMPT);
	expr = expr_eval;

	do	c = io_mgetc(io, 0);
	while	(c == ' ' || c == '\t');

	switch (c)
	{
	case EOF:
	case '\n':
		io_ungetc(c, io);
		break;
	case ':':
		if	(io_peek(io) == ':')
		{
			io_getc(io);
			tcl_name(io);
			expr = expr_var;
		}
		else	tcl_grp(io, c, '\n');

		break;
	case '[':
		expr = expr_expr;
		tcl_grp(io, 0, ']');
		break;
	case '\\':
		io_ungetc(c, io);
		tcl_grp(io, 0, '\n');
		break;
	default:
		tcl_grp(io, c, '\n');
		break;
	}

	io_prompt(io, prompt);
	sb_putc(0, &tcl_buf);
	return Obj_call(expr, etk, NewObjList(str2Obj(sb_memcpy(&tcl_buf))));
}

/*
$SeeAlso
\mref{ETK(3)},
\mref{ETK_eval(3)},
\mref{ETK_func(3)},
\mref{ETK_var(3)},
\mref{SetupETK(3)},
\mref{ETK(7)}.
*/
