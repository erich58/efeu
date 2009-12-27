/*
Dokumenthilfsprogramme

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

#include "DocSym.h"
#include "efeudoc.h"
#include <ctype.h>
#include <EFEU/preproc.h>
#include <EFEU/Resource.h>

#define	HEAD	"document input: $0"
#if	0
#define	SYMTAB	(GetFlagResource("UseUTF8") ? "latin1" : "utf8")
#endif
#define	SYMTAB	"latin1"
#define	HEADCFG	"DocHead"

char *Doc_lastcomment (Doc *doc)
{
	if	(doc && sb_getpos(&doc->buf))
	{
		char *p = sb_strcpy(&doc->buf);
		sb_trunc(&doc->buf);
		return p;
	}
	else	return NULL;
}

void Doc_rem (Doc *doc, const char *fmt, const char *argdef, ...)
{
	va_list list;
	char *p;
	
	va_start(list, argdef);
	p = mpsubvarg(fmt, argdef, list);
	va_end(list);
	io_ctrl(doc->out, DOC_REM, p);
	memfree(p);
}

void Doc_start (Doc *doc)
{
	IO *in;

	if	(doc->stat)	return;

	doc->stat = 1;
	Doc_pushvar(doc);

	in = io_findopen(CFGPath, HEADCFG, CFGEXT, "rd");
	in = io_cmdpreproc(in);
	CmdEval(in, NULL);
	io_close(in);

	io_ctrl(doc->out, DOC_HEAD);
	Doc_popvar(doc);
}

void Doc_stdpar (Doc *doc, int type)
{
	doc->env.par_beg = NULL;
	doc->env.par_end = NULL;
	doc->env.par_type = type;
}


void Doc_hmode (Doc *doc)
{
	if	(!doc->env.hmode)
	{
		Doc_start(doc);

		if	(!doc->env.par_beg)
		{
			if	(doc->env.pflag)
				io_putc('\n', doc->out);

			io_ctrl(doc->out, DOC_BEG, doc->env.par_type);
			doc->env.hmode = 1;
			doc->env.pflag = 0;
		}
		else	doc->env.par_beg(doc);
	}

	if	(doc->env.cpos < doc->env.depth)
	{
		io_nputc(' ', doc->out, doc->env.depth - doc->env.cpos);
		doc->env.cpos = doc->env.depth;
	}
}

void Doc_par (Doc *doc)
{
	if	(doc->env.hmode == 2)	return;

	if	(doc->env.cpos)
	{
		io_putc('\n', doc->out);
		doc->env.cpos = 0;
	}

	if	(doc->env.hmode)
	{
		Doc_nomark(doc);

		if	(!doc->env.par_end)
		{
			io_ctrl(doc->out, DOC_END, doc->env.par_type);
			doc->env.par_type = 0;
			doc->env.hmode = 0;
			doc->env.pflag = 1;
		}
		else	doc->env.par_end(doc);
	}
}

void Doc_newline (Doc *doc)
{
	if	(doc->nl == 0)
	{
		if	(doc->env.cpos)
			io_putc('\n', doc->out);

		doc->env.cpos = 0;
	}
	else	Doc_par(doc);

	doc->nl++;
	doc->indent = 0;
}

void Doc_char (Doc *doc, int c)
{
	Doc_hmode(doc);
	io_putucs(c, doc->out);
	doc->env.cpos++;
}

static int put_str (const char *str, IO *out)
{
	int k;

	for (k = 0; *str; k++)
		io_putucs(pgetucs((char **) &str, 4), out);

	return k;
}


void Doc_str (Doc *doc, const char *str)
{
	if	(str)
	{
		Doc_hmode(doc);
		doc->env.cpos += put_str(str, doc->out);
	}
}

static DocSym *symtab = NULL;

void Doc_symbol (Doc *doc, const char *name, const char *def)
{
	char *sym;
	Doc_hmode(doc);

	if	(!symtab)
		symtab = DocSym_load(SYMTAB);

	if	((sym = DocSym_get(symtab, name)) != NULL)
	{
		doc->env.cpos += put_str(sym, doc->out);
	}
	else
	{
		if	(io_ctrl(doc->out, DOC_SYM, name) == EOF)
			io_puts(def, doc->out);
	
		doc->env.cpos += def ? strlen(def) : 0;
	}
}


int DocSymbol (IO *in, IO *out)
{
	char *name, *sym;
	int c;

	c = io_getc(in);

	if	(isalpha(c))
	{
		io_ungetc(c, in);
		name = DocParseName(in, 0);
		c = '?';
	}
	else if	(c == '#')
	{
		c = io_peek(in);

		if	(c == 'x' || c == 'X')
		{
			io_getc(in);
			c = strtoul(DocParseFlags(in), NULL, 16);
		}
		else if	(isdigit(c))
		{
			c = strtoul(DocParseFlags(in), NULL, 10);
		}
		else	c = '#';

		fprintf(stderr, "Entity: %x\n", c);
		io_putucs(c, out);
		return 1;
	}
	else
	{
		switch (c)
		{
		case ' ':	name = "nbsp"; break;
		case '-':	name = "shy"; break;
		case ';':	return 0;
	/*
		case '|':	io_ctrl(out, DOC_SYM, "_sbr"); return 0;
	*/
		default:	io_putc(c, out); return 1;
		}
	}

	if	(!symtab)	symtab = DocSym_load(SYMTAB);

	if	((sym = DocSym_get(symtab, name)) != NULL)
		return put_str(sym, out);

	if	(io_ctrl(out, DOC_SYM, name) == EOF && c)
		io_putc('?', out);

	return 1;
}
