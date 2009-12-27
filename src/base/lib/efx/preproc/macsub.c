/*
Makrosubstitution

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

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>
#include <EFEU/ioctrl.h>
#include <EFEU/efutil.h>
#include <ctype.h>

#define	PROMPT	"( >>> "

int io_macsub (IO *in, IO *out, const char *delim)
{
	int c, n;

	n = 0;

	while ((c = io_egetc(in, delim)) != EOF)
	{
		if	(isalpha(c) || c == '_')
		{
			n += iocpy_macsub(in, out, c, "!%n_", 1);
		}
		else if	(c =='"')
		{
			n += iocpy_str(in, out, c, "\"", 1);
		}
		else if	(c =='\'')
		{
			n += iocpy_str(in, out, c, "'", 1);
		}
		else if	(io_putc(c, out) != EOF)
		{
			n++;
		}
	}

	return n;
}

int macsub_repl (PPMacro *mac, IO *out, char **arg, int narg)
{
	IO *in;
	int c, n;

	in = io_cstr(mac->repl);
	n = 0;

	while ((c = io_skipcom(in, NULL, 0)) != EOF)
	{
		if	(isalpha(c) || c == '_')
			n += iocpy_macsub(in, out, c, "!%n_", 1);
		else if	(io_putc(c, out) != EOF)
			n++;
	}

	io_close(in);
	return n;
}

int macsub_def (PPMacro *mac, IO *io, char **arg, int narg)
{
	return io_puts(GetMacro(arg[0]) ? "1" : "0", io);
}


int macsub_subst (PPMacro *mac, IO *io, char **arg, int narg)
{
	int i, n;

	for (i = 0; i < mac->dim; i++)
	{
		if	(mac->arg[i] == NULL)	continue;
		if	(i >= narg)		break;

		mac->arg[i]->repl = arg[i];
	}

	PushMacroTab(mac->tab);
	n = macsub_repl(mac, io, NULL, 0);
	PopMacroTab();

	for (i = 0; i < mac->dim; i++)
	{
		if	(mac->arg[i] == NULL)	continue;

		mac->arg[i]->repl = NULL;
	}

	return n;
}

static int nextarg (IO *in, StrBuf *sb)
{
	IO *out;
	int c;

	sb_setpos(sb, 0);
	out = io_strbuf(sb);

	while ((c = io_skipcom(in, NULL, 0)) != EOF)
	{
		if	(isalpha(c) || c == '_')
			iocpy_macsub(in, out, c, "!%n_", 1);
		else if (c == '(')
			iocpy_brace(in, out, c, ")", 1);
		else if (c == '[')
			iocpy_brace(in, out, c, "]", 1);
		else if (c == '{')
			iocpy_brace(in, out, c, "}", 1);
		else if (c == '"')
			iocpy_str(in, out, c, "\"", 1);
		else if (c == '\'')
			iocpy_str(in, out, c, "'", 1);
		else if	(c == ',' || c == ')')
			break;
		else	io_putc(c, out);
	}

	rd_deref(out);
	return c;
}

int iocpy_macsub (IO *in, IO *out, int c,
	const char *arg, unsigned int flags)
{
	int n;
	PPMacro *mac;
	char *prompt;
	StrBuf *sb;
	VecBuf argv;

	sb = new_strbuf(0);
	sb_putc(c, sb);

	while ((c = io_getc(in)) != EOF)
	{
		if	(listcmp(arg, c))
		{
			io_ungetc(c, in);
			break;
		}
		else	sb_putc(c, sb);
	}

	sb_putc(0, sb);
	mac = GetMacro((char *) sb->data);

	if	(mac == NULL || mac->lock)
	{
		n = io_puts((char *) sb->data, out);
		del_strbuf(sb);
		return n;
	}

	vb_init(&argv, 64, sizeof(char *));

	if	(mac->hasarg)
	{
		sb->pos--;
		sb_sync(sb);

		while ((c = io_skipcom(in, NULL, 1)) != EOF)
		{
			if	(c != ' ' && c != '\t')
				break;

			sb_putc(c, sb);
		}

		if	(c != '(')
		{
			io_ungetc(c, in);
			sb_putc(0, sb);
			n = io_puts((char *) sb->data, out);
			del_strbuf(sb);
			return n;
		}

		prompt = io_prompt(in, PROMPT);

		do
		{
			char **p = vb_next(&argv);
			c = nextarg(in, sb);
			sb_putc(0, sb);
			*p = sb_memcpy(sb);
		}
		while	(c == ',');

		io_prompt(in, prompt);
	}

	del_strbuf(sb);
	mac->lock = 1;
	n = mac->sub(mac, out, argv.data, argv.used);
	mac->lock = 0;
	vb_clean(&argv, memfree);
	return n;
}
