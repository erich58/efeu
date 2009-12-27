/*	Befehlsinterpreter
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/cmdeval.h>
#include <ctype.h>


int iocpy_eval(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	Obj_t *obj;
	int n, flag;
	io_t *save_cin;
	io_t *save_cout;

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
			n += PrintObj(out, obj);

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

void io_eval(io_t *io, const char *delim)
{
	iocpy_eval(io, NULL, 0, delim, 0);
}


/*	Befehlszeile interpretieren
*/

void streval(const char *list)
{
	if	(list)
	{
		io_t *io;

		io = io_cstr(list);
		iocpy_eval(io, NULL, 0, NULL, 0);
		io_close(io);
	}
}
