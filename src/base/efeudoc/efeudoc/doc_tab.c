/*
Tabellenzeilen

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include "efeudoc.h"
#include <ctype.h>

static void colend (Doc *doc, int flag)
{
	if	(flag & 2)
		io_ctrl(doc->out, DOC_END, DOC_ENV_MCOL);

	Doc_nomark(doc);
	doc->env.cpos = 0;
}

static void tabline (Doc *doc, IO *in)
{
	int c, flag;
	int space;

	io_ctrl(doc->out, DOC_CMD, DOC_TAB_BEG);
	flag = 0;
	space = 0;

	while ((c = io_skipcom(in, NULL, 0)) != EOF)
	{
		if	(c == doc->env.mark_key && io_peek(in) == '>')
		{
			io_getc(in);
			Doc_endmark(doc);
			flag |= 1;
		}
		else if	(doc->env.cpos == 0 && c == '#')
		{
			int num = DocParseNum(in);
			char *fmt = DocParseFlags(in);
			io_ctrl(doc->out, DOC_BEG, DOC_ENV_MCOL, fmt, num);
			flag |= 2;
		}
		else if	(c == '\n')
		{
			c = io_peek(in);

			if	(c != ' ' && c != '\t')
				break;

			DocSkipSpace(in, 0);
			space = 1;
		}
		else if	(c == '|')
		{
			colend(doc, flag);

			if	(io_ctrl(doc->out, DOC_CMD, DOC_TAB_SEP) == EOF)
				io_putc('\t', doc->out);

			DocSkipSpace(in, 0);
			flag = 0;
			space = 0;
		}
		else
		{
			flag |= 1;

			if	(isspace(c))
			{
				space = 1;
			}
			else
			{
				if	(space)
					Doc_char(doc, ' ');

				Doc_key(doc, in, c);
				space = 0;
			}
		}
	}

	colend(doc, flag);

	if	(io_ctrl(doc->out, DOC_CMD, DOC_TAB_END) == EOF)
		io_putc('\n', doc->out);
}

static void make_line (Doc *doc, IO *in, int typ)
{
	char *p;

	if	((p = DocParseLine(in, 0)))
	{
		while (*p != 0)
		{
			int p1, p2;
			
			while (*p == ' ' || *p == '\t')
				p++;

			if	(!isdigit((unsigned char) *p))
				break;

			p1 = strtoul(p, &p, 10);
			p2 = (*p == '-') ? strtoul(p + 1, &p, 10) : p1;
			io_ctrl(doc->out, DOC_CMD, DOC_TAB_CLINE, typ, p1, p2);
		}
	}
	else	io_ctrl(doc->out, DOC_CMD, DOC_TAB_HLINE, typ);
}

void Doc_tab (Doc *doc, IO *in, const char *opt, const char *arg)
{
	int c;

	Doc_par(doc);
	Doc_newenv(doc, 0, DOC_ENV_TAB, opt, arg);
	doc->env.hmode = 2;

	while ((c = io_skipcom(in, NULL, 1)) != EOF)
	{
		if	(c == '\n')	break;
		if	(isspace(c))	continue;
		
		switch (c)
		{
		case '-':
			if	(io_testkey(in, "--"))
			{
				make_line(doc, in, 1);
				continue;
			}

			break;
		case '=':
			if	(io_testkey(in, "=="))
			{
				make_line(doc, in, 2);
				continue;
			}

			break;
		default:
			break;
		}

		io_ungetc(c, in);
		tabline(doc, in);
	}

	Doc_endenv(doc);
	doc->nl = 1;
	doc->indent = 0;
}
