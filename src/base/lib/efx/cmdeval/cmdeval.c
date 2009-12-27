/*
Befehlszeilen interpretieren

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
#include <EFEU/printobj.h>
#include <EFEU/pconfig.h>
#include <EFEU/cmdeval.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

EfiObj *CmdEval_retval = NULL;
int CmdEval_stat = 0;

IO *CmdEval_cin = NULL;
IO *CmdEval_cout = NULL;

static EfiObj *expr_io (void *par, const EfiObjList *list)
{
	return ConstObj(&Type_io, par);
}

static EfiObj *cpar_io (IO *io, void *data)
{
	return Obj_call(expr_io, data, NULL);
}

static EfiParseDef parse_def[] = {
	{ "cin", cpar_io, &CmdEval_cin },
	{ "cout", cpar_io, &CmdEval_cout },
};


extern void CmdEval_setup (void)
{
	AddParseDef(parse_def, tabsize(parse_def));
}

int CmdPreProc (IO *in, int key)
{
	char *p;

	switch (key)
	{
	case '!':
		io_getc(in);
		p = mpcopy(in, '\n', 0, NULL);
		callproc(p);
		memfree(p);
		return 1;
	default:
		break;
	}

	return 0;
}

void CmdEvalFunc (IO *in, IO *out, int flag)
{
	int at_start, c;
	IO *save_cout;

	save_cout = CmdEval_cout;
	CmdEval_cout = io_refer(out);
	at_start = flag;

	while ((c = io_eat(in, "%s")) != EOF)
	{
		EfiObj *obj;

		if	(c == '{')
		{
			io_getc(in);
			obj = Parse_block(in, '}');
			c = ';';
		}
		else if	(at_start && CmdPreProc(in, c))
		{
			obj = NULL;
			c = '\n';
		}
		else
		{
			obj = Parse_term(in, 0);
			c = io_getc(in);

			if	(isalnum(c) || c == '_')
			{
				io_ungetc(c, in);
				c = ' ';
			}
		}

		CmdEval_stat = 0;
		obj = EvalObj(obj, NULL);

		if	(obj && out && c != ';')
		{
			ShowObj(out, obj);

			if	(c != EOF)
				io_putc(c, out);
		}

		UnrefObj(obj);

		if	(flag)	at_start = (c == '\n');

		if	(CmdEval_stat == CmdEval_Break)	break;
	}

	rd_deref(CmdEval_cout);
	CmdEval_cout = save_cout;
}

void CmdEval (IO *in, IO *out)
{
	IO *save_cin;

	save_cin = CmdEval_cin;
	CmdEval_cin = rd_refer(in);
	CmdEvalFunc(in, out, 1);
	rd_deref(CmdEval_cin);
	CmdEval_cin = save_cin;
}
