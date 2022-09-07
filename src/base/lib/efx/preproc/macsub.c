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
			n += macsub_cpy(in, out, c, 0);
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

static int concat(IO *in)
{
	if	(io_peek(in) != '#')
		return 0;

	int c = io_getc(in);

	if	(io_peek(in) != '#')
	{
		io_ungetc(c, in);
		return 0;
	}

	io_getc(in);
	c = io_peek(in);

	if	(isalnum(c) || c == '_')
		return io_getc(in);

	return 0;
}

static int do_repl (PPMacro *mac, IO *out, int strflag)
{
	IO *in;
	int c, n;
	int flag;
	int quote;

	in = io_cstr(mac->repl);
	n = 0;
	flag = 0;
	quote = 0;

	while ((c = io_skipcom(in, NULL, 0)) != EOF)
	{
		if	(c == '"')
			quote = !quote;

		if	(strflag && c == '#' && !quote)
		{
			flag = 1;
		}
		else if	(!quote && (isalpha(c) || c == '_'))
		{
			if	(flag)
			{
				n += macsub_cpy(in, out, c, flag);
				flag = 0;
			}
			else
			{
				IO *tmp = io_tmpbuf(0);
				macsub_cpy(in, tmp, c, 0);
				int k = 0;

				while ((c = concat(in)))
				{
					macsub_cpy(in, tmp, c, 0);
					++k;
				}

				io_rewind(tmp);

				if	(k)
					n += io_macsub(tmp, out, NULL);
				else
					n += io_copy(tmp, out);

				io_close(tmp);
			}
		}
		else if	(flag && (c == ' ' || c == '\t'))
		{
			;
		}
		else if	(io_putc(c, out) != EOF)
		{
			n++;
			flag = 0;
		}
	}

	io_close(in);
	return n;
}

int macsub_repl (PPMacro *mac, IO *out, char **arg, int narg)
{
	return do_repl(mac, out, 0);
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
	n = do_repl(mac, io, 1);
	PopMacroTab();

	for (i = 0; i < mac->dim; i++)
	{
		if	(mac->arg[i] == NULL)	continue;

		mac->arg[i]->repl = NULL;
	}

	return n;
}

static int nextarg (IO *in, StrBuf *sb, int delim)
{
	IO *out;
	int c;
	int start;

	sb_setpos(sb, 0);
	out = io_strbuf(sb);
	start = 1;

	while ((c = io_skipcom(in, NULL, 0)) != EOF)
	{
		if	(c == ' ' || c == '\t')
		{
			if	(start)
				continue;

			io_putc(c, out);
		}
		else if	(isalpha(c) || c == '_')
			macsub_cpy(in, out, c, 0);
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
		else if	(c == delim || c == ')')
			break;
		else	io_putc(c, out);

		start = 0;
	}

	rd_deref(out);
	return c;
}

int macsub_cpy (IO *in, IO *out, int c, unsigned int flags)
{
	int n;
	PPMacro *mac;
	const char *prompt;
	const char *name;
	StrBuf *sb;
	VecBuf argv;

	sb = sb_acquire();
	sb_putc(c, sb);

	while ((c = io_getc(in)) != EOF)
	{
		if	(listcmp("!%n_", c))
		{
			io_ungetc(c, in);
			break;
		}
		else	sb_putc(c, sb);
	}

	name = sb_nul(sb);
	mac = GetMacro(name);

	if	(mac == NULL || mac->lock)
	{
		n = io_puts(name, out);
		sb_release(sb);
		return n;
	}

	if	(flags)
	{
		io_putc('"', out);
		n = io_xputs(mac->repl, out, "\"");
		io_putc('"', out);
		return n + 2;
	}

	vb_init(&argv, 64, sizeof(char *));

	if	(mac->hasarg)
	{
		while ((c = io_skipcom(in, NULL, 1)) != EOF)
		{
			if	(c != ' ' && c != '\t')
				break;

			sb_putc(c, sb);
		}

		if	(c != '(')
		{
			io_ungetc(c, in);
			n = io_puts(sb_nul(sb), out);
			sb_release(sb);
			return n;
		}

		prompt = io_prompt(in, PROMPT);
		int n = 0;

		do
		{
			++n;
			c = nextarg(in, sb,
				(mac->vaarg && n >= mac->dim) ? EOF : ',');
			sb_putc(0, sb);
			char **p = vb_next(&argv);
			*p = sb_memcpy(sb);
		}
		while	(c == ',');

		io_prompt(in, prompt);
	}

	sb_release(sb);
	mac->lock = 1;
	n = mac->sub(mac, out, argv.data, argv.used);
	mac->lock = 0;
	vb_clear(&argv, memfree);
	return n;
}
