/*
Befehlsinterpreter als Eingabefilter

$Copyright (C) 1999 Erich Frühstück
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
#include <EFEU/preproc.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

typedef struct {
	IO *io;	/* Eingabestruktur */
	StrBuf *buf;	/* Zwischenbuffer */
	char *delim;	/* Abschlußzeichen */
	IO *cin;	/* Zwischensicherung von CmdEval_cin */
	IO *cout;	/* Zwischensicherung von CmdEval_cout */
	unsigned save : 31;	/* Zahl der gespeicherten Zeichen */
	unsigned eof : 1; 	/* EOF - Flag */
} CEPAR;

static int ce_get (void *ptr)
{
	CEPAR *par = ptr;
	EfiObj *obj;
	IO *io;
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
				ShowObj(CmdEval_cout, obj);
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

static int ce_ctrl (void *ptr, int req, va_list list)
{
	CEPAR *par = ptr;
	int stat;

	switch (req)
	{
	case IO_CLOSE:
		io_close(CmdEval_cout);
		CmdEval_cout = par->cout;
		CmdEval_cin = par->cin;
		stat = io_close(par->io);
		sb_destroy(par->buf);
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

IO *io_cmdeval (IO *io, const char *delim)
{
	if	(io != NULL)
	{
		CEPAR *par = memalloc(sizeof(CEPAR));
		par->io = io;
		par->delim = mstrcpy(delim);
		par->buf = sb_create(0);
		par->save = 0;
		par->eof = 0;
		par->cin = CmdEval_cin;
		CmdEval_cin = par->io;

		par->cout = CmdEval_cout;
		CmdEval_cout = io_strbuf(par->buf);

		io = io_alloc();
		io->get = ce_get;
		io->ctrl = ce_ctrl;
		io->data = par;
	}

	return io;
}
