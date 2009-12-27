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
#include <EFEU/EfeuConfig.h>
#include <EFEU/strbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/parsub.h>
#include <EFEU/procenv.h>
#include <ctype.h>

#define	HLP_PFX	"help"
#define	HLP_SFX	"hlp"

#define	USAGE_FMT	"@synopsis -h\n"

#define	HLP_FMT	"@name\n\
@synopsis -h\n\
@arglist -hi\n\
@environ -h\n\
@copyright -h\n"

#define	STD_OUT		io_popen("efeudoc -t -", "w")
#define	ERR_OUT		io_popen("efeudoc -Tterm - 1>&2", "w")


static void expand_func (CmdPar *par, IO *in, IO *out, int c)
{
	StrBuf *sb;
	CmdParExpand *eval;
	char *arg;

	sb = sb_acquire();

	while (!isspace(c) && c != EOF)
	{
		sb_putc(c, sb);
		c = io_getc(in);
	}

	arg = sb_nul(sb);
	eval = CmdParExpand_get(arg);

	if	(eval == NULL)
	{
		io_ungetc(c, in);
		io_putc('@', out);
		io_puts(arg, out);
		sb_release(sb);
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

	arg = sb_getpos(sb) ? sb_nul(sb) : NULL;
	eval->eval(par, out, arg);
	sb_release(sb);
}

static void subcopy (CmdPar *par, IO *in, IO *out, int delim);

static void expand_var (CmdPar *par, IO *in, IO *out, int key)
{
	StrBuf *sb;
	char *name;
	IO *io;

	sb = sb_acquire();
	io = io_strbuf(sb);
	subcopy(par, in, io, key);
	io_close(io);
	name = sb_getpos(sb) ? sb_nul(sb) : NULL;
	io_puts(CmdPar_getval(par, name, NULL), out);
	sb_release(sb);
}

void CmdPar_expand (CmdPar *par, IO *in, IO *out)
{
	int32_t c;

	if	(par == NULL)
	{
		io_putc('@', out);
		return;
	}

	c = io_getucs(in);

	if	(c < 256 && isspace(c))
	{
		io_ungetc(c, in);
		io_putc('@', out);
	}
	else if	(c == '!')
	{
		io_xprintf(out, "|%s|", par->name);
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

static void subcopy (CmdPar *par, IO *in, IO *out, int delim)
{
	int32_t c;

	while ((c = io_getucs(in)) != delim)
	{
		switch (c)
		{
		case EOF:
			return;
		case '$':
			io_puts(psubexpand(NULL, in, 0, NULL), out);
			break;
		case '@':
			CmdPar_expand(par, in, out);
			break;
		default:
			io_putucs(c, out);
			break;
		}
	}
}


void CmdPar_iousage (CmdPar *par, IO *out, IO *def)
{
	if	(out && def)
		subcopy(CmdPar_ptr(par), def, out, EOF);
}

void CmdPar_usage (CmdPar *par, IO *out, const char *fmt)
{
	IO *def = io_cstr(fmt ? fmt : USAGE_FMT);

	if	(out)
		out = io_refer(out);
	else if	(EfeuConfigPar.standalone)
		out = io_refer(ioerr);
	else	out = ERR_OUT;

	subcopy(CmdPar_ptr(par), def, out, EOF);
	io_close(def);
	io_close(out);
}

char *CmdPar_hpath (CmdPar *par)
{
	par = CmdPar_ptr(par);
	return fsearch(ApplPath, HLP_PFX, par->name, HLP_SFX);
}

void CmdPar_manpage (CmdPar *par, IO *out)
{
	char *p;
	IO *def;

	par = CmdPar_ptr(par);
	p = fsearch(ApplPath, HLP_PFX, par->name, HLP_SFX);

	if	(!p)
	{
		char *base = mbasename(par->name, &p);
		p = fsearch(ApplPath, HLP_PFX, base, HLP_SFX);
		memfree(base);
	}

	if	(p)
	{
		def = io_fileopen(p, "r");
		memfree(p);
	}
	else	def = io_cstr(CmdPar_getval(par, ".manpage.fmt", NULL));

	if	(out)
		out = io_refer(out);
	else if	(EfeuConfigPar.standalone)
		out = io_refer(iostd);
	else	out = STD_OUT;

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


