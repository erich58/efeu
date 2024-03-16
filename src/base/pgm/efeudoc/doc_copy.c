/*
Standardformatierung

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


static void copy_verb (Doc *doc, IO *in)
{
	Doc_hmode(doc);
	doc->env.cpos += DocVerb(in, doc->out);
}

/*
void Doc_psub (Doc *doc, IO *in)
{
	char *p = DocParseExpr(in);
	Doc_eval(doc, in, p);
	memfree(p);
}
*/

static char *copy_item (Doc *doc, IO *in, int beg, int end)
{
	StrBuf *buf = sb_acquire();
	char *p;
	int depth, c;
	
	doc->indent++;
	depth = 0;

	while ((c = io_getc(in)) != EOF)
	{
		doc->indent++;

		if	(c == end)
		{
			if	(depth <= 0)	break;
			else			depth--;
		}
		else if	(c == beg)	depth++;

		switch (c)
		{
		case '\\':
			c = io_getc(in);

			if	(c == beg || c == end || c == '\\')
			{
				doc->indent++;
				sb_putc(c, buf);
			}
			else
			{
				io_ungetc(c, in);
				sb_putc('\\', buf);
				c = '\\';
			}

			break;
		case '\n':
			doc->indent = 0;
			sb_putc(' ', buf);
			break;
		case '\t':
			doc->indent = 8 * ((doc->indent + 7) / 8);
			sb_putc(' ', buf);
			break;
		default:
			sb_putc(c, buf);
			break;
		}
	}

	sb_putc(0, buf);
	p = sb_memcpy(buf);
	sb_release(buf);
	return p;
}

void Doc_sig (Doc *doc, int sig, IO *in)
{
	int c = io_peek(in);

	Doc_hmode(doc);

	if	(isdigit(c))
	{
		io_ctrl(doc->out, DOC_BEG, DOC_ENV_FORMULA);
		Doc_char(doc, sig);
		io_ctrl(doc->out, DOC_END, DOC_ENV_FORMULA);
	}
	else	Doc_char(doc, sig);
}


void Doc_minus (Doc *doc, IO *in)
{
	if	(io_peek(in) == '-')
	{
		io_getc(in);

		if	(io_peek(in) == '-')
		{
			io_getc(in);
			Doc_symbol(doc, "mdash", "-");
		}
		else	Doc_symbol(doc, "ndash", "-");
	}
	else	Doc_sig (doc, '-', in);
}

void Doc_dots (Doc *doc, IO *in)
{
	if	(io_peek(in) == '.')
	{
		io_getc(in);

		if	(io_peek(in) == '.')
		{
			io_getc(in);
			Doc_symbol(doc, "ldots", "...");
			return;
		}

		Doc_char(doc, '.');
	}

	Doc_char(doc, '.');
}

static int doc_space (Doc *doc, int c)
{
	switch (c)
	{
	case 0:
	case 127:
	case '\b':
	case '\r':
	case '\f':
	case '\v':
		return 1;
	case '\n':
		Doc_newline(doc);
		return 1;
	case '\t':
		if	(doc->nl == 0)
		{
			do	Doc_char(doc, ' ');
			while	(doc->env.cpos % 8 != 0);
		}
		else	doc->indent = 8 * (1 + doc->indent / 8);
		return 1;
	case ' ':
		if	(doc->nl)	doc->indent++;
		else			Doc_char(doc, ' ');

		return 1;
	default:
		return 0;
	}
}

void Doc_key (Doc *doc, IO *in, int c)
{
	switch (c)
	{
	case '\\':	Doc_cmd(doc, in); break;
	case '@':	Doc_mac(doc, in); break;
/*
	case '$':	Doc_psub(doc, in); break;
*/
	case '|':	copy_verb(doc, in); break;
	case '&':	Doc_hmode(doc);
			doc->env.cpos += DocSymbol(in, doc->out); break;
	case '~':	Doc_symbol(doc, "nbsp", " "); break;
	case '<':	Doc_begmark(doc, in); break;
	case '>':	Doc_endmark(doc); break;
	case '+':	Doc_sig(doc, c, in); break;
	case '-':	Doc_minus(doc, in); break;
	case '.':	Doc_dots(doc, in); break;
	default:	Doc_char(doc, io_ucscompose(in, c)); break;
	}
}

void Doc_xcopy (Doc *doc, IO *in, int delim)
{
	int c;

	while ((c = io_skipcom(in, &doc->buf, doc->env.cpos == 0)) != EOF)
	{
		if	(c == delim)
		{
			return;
		}
		else if	(c == doc->env.mark_key && io_peek(in) == '>')
		{
			io_getc(in);
			Doc_endmark(doc);
		}
		else if	(isspace(c))
		{
			Doc_char(doc, ' ');
		}
		else	Doc_key(doc, in, c);
	}
}

static int input_mark(IO *in)
{
	int n, c;

	for (n = 0; (c = io_getc(in)) == '-'; n++)
		;

	io_ungetc(c, in);

	if	(n < 3)
	{
		while	(n-- > 0)
			io_ungetc('-', in);

		return 0;
	}
	else	return 1;
}

void Doc_copy (Doc *doc, IO *in)
{
	int c;

	while ((c = io_skipcom(in, &doc->buf, doc->env.cpos == 0)) != EOF)
	{
		if	(doc_space(doc, c))
			continue;
		
		if	(c == doc->env.mark_key && io_peek(in) == '>')
		{
			doc->nl = 0;
			io_getc(in);
			Doc_endmark(doc);
			continue;
		}

		if	(doc->nl)
		{
			while (doc->indent < doc->env.indent)
				Doc_endenv(doc);

			switch (c)
			{
			case '-':
				if	(input_mark(in))
				{
					char *opt, *arg;

					opt = io_mgets(in, "\n");
					io_getc(in);
					arg = DocParseRegion(in, "----*");
					Doc_input(doc, opt, io_mstr(arg));
					memfree(opt);
					continue;
				}
				else	break;

			case '*':
				if	(!isspace(io_peek(in)))
					break;
					
				Doc_item(doc, DOC_LIST_ITEM);
/*
				doc->nl = 0;
*/
				doc->indent++;
				continue;
			case '#':
				if	(!isspace(io_peek(in)))
					break;
					
				Doc_item(doc, DOC_LIST_ENUM);
/*
				doc->nl = 0;
*/
				doc->indent++;
				continue;
			case '[':
				Doc_item(doc, DOC_LIST_DESC);
/*
				doc->nl = 0;
*/
				doc->env.item = copy_item(doc, in, c, ']');
				continue;
			default:
				break;
			}

			if	(doc->indent > doc->env.indent)
			{
				if	(!DOC_IS_LIST(doc->env.type))
					Doc_newenv(doc, 4, DOC_ENV_QUOTE, NULL);
			}
			else if	(doc->nl > 1 && DOC_IS_LIST(doc->env.type))
			{
				Doc_endenv(doc);
				Doc_par(doc);
			}
		}

		doc->nl = 0;
		Doc_key(doc, in, c);
	}
}

void Doc_scopy (Doc *doc, const char *str)
{
	IO *in = io_cstr(str);
	Doc_copy(doc, in);
	io_close(in);
}

void Doc_mcopy (Doc *doc, char *str)
{
	IO *in = io_mstr(str);
	Doc_copy(doc, in);
	io_close(in);
}
