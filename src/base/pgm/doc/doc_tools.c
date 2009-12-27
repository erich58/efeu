/*	Dokumenthilfsprogramme
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "DocSym.h"
#include "efeudoc.h"
#include <ctype.h>

#define	HEAD	"Dokumenteingabe: $0"
#define	SYMTAB	"latin1"

char *Doc_lastcomment (Doc_t *doc)
{
	if	(doc && doc->buf && sb_getpos(doc->buf))
	{
		char *p = mstrncpy((char *) doc->buf->data,
			sb_getpos(doc->buf));
		sb_clear(doc->buf);
		return p;
	}
	else	return NULL;
}

void Doc_rem (Doc_t *doc, const char *fmt)
{
	char *p = parsub(fmt);
	io_ctrl(doc->out, DOC_REM, p);
	memfree(p);
}

void Doc_start (Doc_t *doc)
{
	if	(doc->stat)	return;

	Doc_pushvar(doc);
	io_ctrl(doc->out, DOC_HEAD);
	Doc_popvar(doc);
	doc->stat = 1;
}

void Doc_stdpar (Doc_t *doc, int type)
{
	doc->env.par_beg = NULL;
	doc->env.par_end = NULL;
	doc->env.par_type = type;
}


void Doc_hmode (Doc_t *doc)
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

void Doc_par (Doc_t *doc)
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

void Doc_newline (Doc_t *doc)
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

void Doc_char (Doc_t *doc, int c)
{
	Doc_hmode(doc);
	io_putc(c, doc->out);
	doc->env.cpos++;
}

void Doc_str (Doc_t *doc, const char *str)
{
	if	(str)
	{
		Doc_hmode(doc);
		doc->env.cpos += io_puts(str, doc->out);
	}
}

static DocSym_t *symtab = NULL;

void Doc_symbol (Doc_t *doc, const char *name, const char *def)
{
	char *sym;
	Doc_hmode(doc);

	if	(!symtab)	symtab = DocSym(SYMTAB);

	if	((sym = DocSym_get(symtab, name)) != NULL)
	{
		doc->env.cpos += io_puts(sym, doc->out);
	}
	else
	{
		if	(io_ctrl(doc->out, DOC_SYM, name) == EOF)
			io_puts(def, doc->out);
	
		doc->env.cpos += def ? strlen(def) : 0;
	}
}


int DocSymbol (io_t *in, io_t *out)
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

	if	(!symtab)	symtab = DocSym(SYMTAB);

	if	((sym = DocSym_get(symtab, name)) != NULL)
		return io_puts(sym, out);

	if	(io_ctrl(out, DOC_SYM, name) == EOF && c)
		io_putc('?', out);

	return 1;
}
