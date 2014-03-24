/*
Befehlsinterpreter

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
#include <EFEU/cmdeval.h>
#include <ctype.h>


int iocpy_eval(IO *in, IO *out, int c, const char *arg, unsigned int flags)
{
	EfiObj *obj;
	int n, flag;
	IO *save_cin;
	IO *save_cout;

	save_cin = CmdEval_cin;
	save_cout = CmdEval_cout;
	CmdEval_cin = rd_refer(in);
	CmdEval_cout = rd_refer(out);

	n = 0;
	flag = 1;

	while (flag && (c = io_eat(in, "%s")) != EOF)
	{
		if	(listcmp(arg, c))
		{
			io_getc(in);
			break;
		}
		else if	(c == '{')
		{
			io_getc(in);
			obj = Parse_block(in, '}');
		}
		else	obj = Parse_term(in, 0);

		c = io_getc(in);

		if	(c == EOF || listcmp(arg, c))
		{
			flag = 0;
		}
		else if	(listcmp("%n_\"'{", c))
		{
			io_ungetc(c, in);
			c = ' ';
		}

		obj = EvalObj(obj, NULL);

		if	(obj && out && c != ';')
		{
			n += ShowObj(out, obj);

			if	(flag)
				n += io_nputc(c, out, 1);
		}

		UnrefObj(obj);
	}

	rd_deref(CmdEval_cin);
	rd_deref(CmdEval_cout);
	CmdEval_cin = save_cin;
	CmdEval_cout = save_cout;
	return n;
}

void io_eval(IO *io, const char *delim)
{
	iocpy_eval(io, NULL, 0, delim, 0);
}


/*	Befehlszeile interpretieren
*/

void streval(const char *list)
{
	if	(list)
	{
		IO *io;

		io = io_cstr(list);
		iocpy_eval(io, NULL, 0, NULL, 0);
		io_close(io);
	}
}
