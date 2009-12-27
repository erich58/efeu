/*
Aufrufsyntax aus Kommandoparameter generieren

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/CmdPar.h>
#include <EFEU/strbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/parsub.h>
#include <EFEU/procenv.h>
#include <ctype.h>

#define	HLP_PFX	"help"
#define	HLP_SFX	"hlp"

#define	USAGE_FMT	"\\Synopsis\n@synopsis\n"

#define	HLP_FMT	"@head\n\\Name\n@ident\n\
\\Synopsis\n@synopsis usage:\n\
\\Description\n@arglist\n\
\\Environment\n@environ\n"

#define	STD_OUT		io_popen("efeudoc -t -", "w")
#define	ERR_OUT		io_popen("efeudoc -Tterm - 1>&2", "w")


static void expand_func (CmdPar_t *par, io_t *in, io_t *out, int c)
{
	strbuf_t *sb;
	CmdParExpand_t *eval;
	char *arg;

	sb = new_strbuf(0);

	while (!isspace(c) && c != EOF)
	{
		sb_putc(c, sb);
		c = io_getc(in);
	}

	sb_putc(0, sb);
	eval = CmdParExpand_get((char *) sb->data);

	if	(eval == NULL)
	{
		io_ungetc(c, in);
		io_putc('@', in);
		io_puts((char *) sb->data, out);
		sb_delete(sb);
		return;
	}

	sb_begin(sb);

	while	(c == ' ' || c == '\t')
		c = io_getc(in);

	while (c != EOF && c != '\n')
	{
		sb_putc(c, sb);
		c = io_getc(in);
	}

	if	(sb_getpos(sb))
	{
		sb_putc(0, sb);
		arg = (char *) sb->data;
	}
	else	arg = NULL;

	eval->eval(par, out, arg);
	del_strbuf(sb);
}

static void subcopy (CmdPar_t *par, io_t *in, io_t *out, int delim);

static void expand_var (CmdPar_t *par, io_t *in, io_t *out, int key)
{
	strbuf_t *sb;
	char *name;
	io_t *io;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	subcopy(par, in, io, key);
	io_close(io);

	if	(sb_getpos(sb))
	{
		sb_putc(0, sb);
		name = (char *) sb->data;
	}
	else	name = NULL;

	io_puts(CmdPar_getval(par, name, NULL), out);
	sb_delete(sb);
}

static void expand (CmdPar_t *par, io_t *in, io_t *out)
{
	int c = io_getc(in);

	if	(isspace(c))
	{
		io_ungetc(c, in);
		io_putc('@', out);
	}
	else if	(c == '!')
	{
		io_printf(out, "|%s|", par->name);
	}
	else if	(c == '{')
	{
		expand_var(par, in, out, '}');
	}
	else if	(c == EOF || c == '@')
	{
		io_putc('@', out);
	}
	else	expand_func(par, in, out, c);
}

static void subcopy (CmdPar_t *par, io_t *in, io_t *out, int delim)
{
	int c;

	while ((c = io_getc(in)) != delim)
	{
		switch (c)
		{
		case EOF:
			return;
		case '$':
			iocpy_psub(in, out, c, NULL, 0);
			break;
		case '@':
			expand(par, in, out);
			break;
		default:
			io_putc(c, out);
			break;
		}
	}
}


void CmdPar_iousage (CmdPar_t *par, io_t *out, io_t *def)
{
	if	(out && def)
		subcopy(CmdPar_ptr(par), def, out, EOF);
}

void CmdPar_usage (CmdPar_t *par, io_t *out, const char *fmt)
{
	io_t *def = io_cstr(fmt ? fmt : USAGE_FMT);
	out = out ? io_refer(out) : ERR_OUT;
	subcopy(CmdPar_ptr(par), def, out, EOF);
	io_close(def);
	io_close(out);
}

void CmdPar_manpage (CmdPar_t *par, io_t *out)
{
	char *p;
	io_t *def;

	par = CmdPar_ptr(par);
	p = xfsearch(ApplPath, HLP_PFX, par->name, HLP_SFX);
	def = p ? io_fileopen(p, "r") : io_cstr(HLP_FMT);
	memfree(p);
	out = out ? io_refer(out) : STD_OUT;
	subcopy(par, def, out, EOF);
	io_close(def);
	io_close(out);
}

/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
\mref{CmdParExpand(3)},
\mref{CmdParKey(3)},
\mref{CmdParVar(3)},
\mref{CmdPar_eval(3)},
\mref{CmdPar_list(3)},
\mref{CmdPar_load(3)},
\mref{CmdPar_psub(3)},
\mref{CmdPar_write(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/


