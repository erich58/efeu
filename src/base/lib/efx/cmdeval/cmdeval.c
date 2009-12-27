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

Obj_t *CmdEval_retval = NULL;
int CmdEval_stat = 0;

io_t *CmdEval_cin = NULL;
io_t *CmdEval_cout = NULL;

static Obj_t *expr_io (void *par, const ObjList_t *list)
{
	return ConstObj(&Type_io, par);
}

static Obj_t *cpar_io (io_t *io, void *data)
{
	return Obj_call(expr_io, data, NULL);
}

/*
static Obj_t *cpar_cin (io_t *io, void *data)
{
	return ConstObj(&Type_io, &CmdEval_cin);
}

static Obj_t *cpar_cout (io_t *io, void *data)
{
	return ConstObj(&Type_io, &CmdEval_cout);
}
*/

static ParseDef_t parse_def[] = {
	/*
	{ "cin", cpar_cin, NULL },
	{ "cout", cpar_cout, NULL },
	*/
	{ "cin", cpar_io, &CmdEval_cin },
	{ "cout", cpar_io, &CmdEval_cout },
};


extern void CmdEval_setup (void)
{
	AddParseDef(parse_def, tabsize(parse_def));
}

static iocpy_t cmdline[] = {
	{ "\n", NULL, 0, NULL },
	{ "$", NULL, 0, iocpy_psub },
};

static int c_system(io_t *in)
{
	char *p;

	io_getc(in);
	p = miocpy(in, cmdline, tabsize(cmdline));

	if	(p != NULL)
	{
		system(p);
		memfree(p);
	}
	else if	(Shell != NULL)
	{
		system(Shell);
	}

	return 1;
}

/*
static int c_preproc(io_t *in)
{
	char *p;

	p = miocpy(in, cmdline, tabsize(cmdline));
	io_printf(ioerr, "PPLINE\t%s\n", p);
	memfree(p);
	return 1;
}
*/

int CmdPreProc (io_t *in, int key)
{
	switch (key)
	{
	case '!':	return c_system(in);
/*
	case '#':	return c_preproc(in);
*/
	default:	break;
	}

	return 0;
}

void CmdEvalFunc(io_t *in, io_t *out, int flag)
{
	int at_start, c;
	io_t *save_cout;

	save_cout = CmdEval_cout;
	CmdEval_cout = io_refer(out);
	at_start = flag;

	while ((c = io_eat(in, "%s")) != EOF)
	{
		Obj_t *obj;

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
			PrintObj(out, obj);

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

void CmdEval(io_t *in, io_t *out)
{
	io_t *save_cin;

	save_cin = CmdEval_cin;
	CmdEval_cin = rd_refer(in);
	CmdEvalFunc(in, out, 1);
	rd_deref(CmdEval_cin);
	CmdEval_cin = save_cin;
}
