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


static void tabcmd (Doc_t *doc, char *arg)
{
	char  *p;

	for (p = arg; *p != 0; p++)
	{
		switch (*p)
		{
		case '@':
			io_ctrl(doc->out, DOC_CMD, DOC_CMD_TEX, "\\emptyline\n");
			break;
		case '-':
			io_ctrl(doc->out, DOC_CMD, DOC_TAB_BRULE);
			break;
		case '*':
			io_ctrl(doc->out, DOC_CMD, DOC_TAB_BLINE);
			break;
		case '+':
			io_ctrl(doc->out, DOC_CMD, DOC_TAB_INDENT);
			break;
		default:
			if	(isdigit(*p))
			{
				io_ctrl(doc->out, DOC_CMD,
					DOC_TAB_HEIGHT, strtol(p, &p, 10));
				p--;
			}

			break;
		}
	}
	
	memfree(arg);
}

static void colend (Doc_t *doc, int flag)
{
	if	(flag & 2)
		io_ctrl(doc->out, DOC_END, DOC_ENV_MCOL);
	else if	(flag == 0)
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_TEX, "\\norule");

	Doc_nomark(doc);
	doc->env.cpos = 0;
}

static void tabline (Doc_t *doc, io_t *in)
{
	int c, flag;
	char *arg;

	while ((arg = DocParseArg(in, '[', ']', 1)) != NULL)
		tabcmd(doc, arg);

	c = io_peek(in);

	if	(c == '\n' || c == EOF)
	{
		io_getc(in);
		/*
		io_ctrl(doc->out, DOC_CMD, DOC_CMD_TEX, "\\emptyline\n");
		*/
		return;
	}

	io_ctrl(doc->out, DOC_CMD, DOC_TAB_BEG);
	flag = 0;

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
		}
		else if	(c == '|')
		{
			colend(doc, flag);
			io_ctrl(doc->out, DOC_CMD, DOC_TAB_SEP);
			DocSkipSpace(in, 0);
			flag = 0;
		}
		else
		{
			flag |= 1;

			if	(isspace(c))
			{
				Doc_char(doc, ' ');
			}
			else	Doc_key(doc, in, c);
		}
	}

	colend(doc, flag);
	io_ctrl(doc->out, DOC_CMD, DOC_TAB_END);
	io_putc('\n', doc->out);
}

void Doc_tab (Doc_t *doc, io_t *in, const char *opt, const char *arg)
{
	int c;

	Doc_par(doc);
	Doc_newenv(doc, 0, DOC_ENV_TAB, opt, arg);
	doc->env.hmode = 2;

	while ((c = io_skipcom(in, NULL, 1)) != EOF)
	{
		if	(c == '\n')	break;
		if	(isspace(c))	continue;
		
		io_ungetc(c, in);
		tabline(doc, in);
	}

	Doc_endenv(doc);
	doc->nl = 1;
	doc->indent = 0;
}
