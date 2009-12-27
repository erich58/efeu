/*	Befehlsinterpreter als Eingabefilter
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/cmdeval.h>
#include <EFEU/preproc.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

typedef struct {
	io_t *io;	/* Eingabestruktur */
	strbuf_t *buf;	/* Zwischenbuffer */
	char *delim;	/* Abschlußzeichen */
	io_t *cin;	/* Zwischensicherung von CmdEval_cin */
	io_t *cout;	/* Zwischensicherung von CmdEval_cout */
	unsigned save : 31;	/* Zahl der gespeicherten Zeichen */
	unsigned eof : 1; 	/* EOF - Flag */
} CEPAR;

static int ce_get (CEPAR *par)
{
	Obj_t *obj;
	io_t *io;
	int c;

	while (par->save == 0)
	{
		if	(par->eof)	return EOF;

		io = par->io;
		c = io_eat(io, "%s");
		sb_begin(par->buf);

		if	(c == EOF)	return EOF;

		if	(c == '{')
		{
			io_getc(io);
			obj = Parse_block(io, '}');
			io_eat(io, " \t");
		}
		else	obj = Parse_term(io, 0);

		c = io_getc(io);

		if	(c == EOF)
		{
			par->eof = 1;
		}
		else if	(listcmp(par->delim, c))
		{
			io_ungetc(c, io);
			c = EOF;
			par->eof = 1;
		}
		else if	(listcmp("%n_\"'{", c))
		{
			io_ungetc(c, io);
			c = ' ';
		}

		obj = EvalObj(obj, NULL);

		if	(c != ';')
		{
			if	(obj)
				PrintObj(CmdEval_cout, obj);
			else if	(isspace(c))
				c = EOF;
			
			if	(c != EOF)
				sb_putc(c, par->buf);
		}

		UnrefObj(obj);
		par->save = sb_getpos(par->buf);
		sb_begin(par->buf);
	}

	par->save--;
	return sb_getc(par->buf);
}

static int ce_ctrl (CEPAR *par, int req, va_list list)
{
	int stat;

	switch (req)
	{
	case IO_CLOSE:
		io_close(CmdEval_cout);
		CmdEval_cout = par->cout;
		CmdEval_cin = par->cin;
		stat = io_close(par->io);
		del_strbuf(par->buf);
		memfree(par->delim);
		memfree(par);
		break;
		return EOF;
	case IO_IDENT:
		*va_arg(list, char **) = io_xident(par->io, "cmdeval(%*)");
		return 0;
	default:
		stat = io_vctrl(par->io, req, list);
		break;
	}

	return stat;
}

io_t *io_cmdeval (io_t *io, const char *delim)
{
	if	(io != NULL)
	{
		CEPAR *par = memalloc(sizeof(CEPAR));
		par->io = io;
		par->delim = mstrcpy(delim);
		par->buf = new_strbuf(0);
		par->save = 0;
		par->eof = 0;
		par->cin = CmdEval_cin;
		CmdEval_cin = par->io;

		par->cout = CmdEval_cout;
		CmdEval_cout = io_strbuf(par->buf);

		io = io_alloc();
		io->get = (io_get_t) ce_get;
		io->ctrl = (io_ctrl_t) ce_ctrl;
		io->data = par;
	}

	return io;
}
