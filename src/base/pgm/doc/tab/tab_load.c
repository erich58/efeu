/*
Befehlsdefinitionen laden

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

#include <efeudoc.h>
#include <ctype.h>
#include <EFEU/printobj.h>
#include <EFEU/preproc.h>
#include <EFEU/cmdeval.h>

#define	FILE_EXT	"dmac"
#define	PROTECT		"protect"
#define	BREAK		"break"

#define FMT	"[ftools:6]$!: file $1 not found.\n"

/*	Ladebefehle
*/

typedef void (*LoadFunc) (DocTab *tab, IO *in, StrBuf *desc);

static char *get_data (StrBuf *buf)
{
	return buf->pos ? mstrncpy((char *) buf->data, buf->pos) : NULL;
}


static void cmd_eval (DocTab *tab, IO *in, StrBuf *desc)
{
	IO *io = Doc_preproc(io_mstr(DocParseExpr(in)));
	CmdEvalFunc(io, NULL, 0);
	io_close(io);
}

static void cmd_load (DocTab *tab, IO *in, StrBuf *desc)
{
	char *p = DocParseLine(in, 0);
	DocTab_fload(tab, p);
	memfree(p);
}

static char *get_desc (StrBuf *buf)
{
	IO *io;
	StrBuf *desc;
	int c;

	sb_setpos(buf, 0);
	io = langfilter(io_strbuf(buf), NULL);
	desc = sb_create(0);

	while ((c = io_getc(io)) != EOF)
		sb_putc(c, desc);

	io_close(io);
	return sb2str(desc);
}

void DocTab_def (DocTab *tab, IO *in, StrBuf *buf)
{
	char *name;
	
	switch (DocSkipSpace(in, 0))
	{
	case '@':
	case '\\':
		name = mstrcpy(DocParseName(in, io_getc(in)));
		DocTab_setmac(tab, name, get_desc(buf), DocParseExpr(in));
		break;
	default:
		io_note(in, "[Doc:21]", NULL);
		memfree(DocParseLine(in, 0));
		break;
	}
}

static void cmd_show (DocTab *tab, IO *in, StrBuf *desc)
{
	char *p = get_data(desc);
	io_printf(ioerr, "---- show\n%s****\n", p);
	memfree(p);
}

static void cmd_if (DocTab *tab, IO *in, StrBuf *desc)
{
	IO *io;
	int mode;
	
	io = Doc_preproc(io_mstr(DocParseLine(in, 0)));
	mode = Obj2bool(Parse_term(io, 0));
	io_close(io);
	io_push(in, io_mstr(DocParseBlock(in, mode, "if*", "endif", "else")));
}

static int is_defined (DocTab *tab, IO *in)
{
	char *name;

	switch (DocSkipSpace(in, 0))
	{
	case '@':
	case '\\':
		name = DocParseName(in, io_getc(in));
		return DocTab_getmac(tab, name) != NULL;
	default:
		break;
	}

	return 0;
}

static void cmd_ifdef (DocTab *tab, IO *in, StrBuf *desc)
{
	int mode = is_defined(tab, in);
	io_push(in, io_mstr(DocParseBlock(in, mode, "if*", "endif", "else")));
}

static void cmd_ifndef (DocTab *tab, IO *in, StrBuf *desc)
{
	int mode = !is_defined(tab, in);
	io_push(in, io_mstr(DocParseBlock(in, mode, "if*", "endif", "else")));
}

static struct {
	char *name;
	LoadFunc func;
} ctab[] = {
	{ "eval",	cmd_eval },
	{ "def",	DocTab_def },
	{ "include",	cmd_load },
	{ "show",	cmd_show },
	{ "if",		cmd_if },
	{ "ifdef",	cmd_ifdef },
	{ "ifndef",	cmd_ifndef },
};

static LoadFunc get_func (const char *name)
{
	int i;

	for (i = 0; i < tabsize(ctab); i++)
		if	(mstrcmp(ctab[i].name, name) == 0)
			return ctab[i].func;

	return NULL;
}


static void copy_verb (IO *in, StrBuf *buf)
{
	int c;
	
	while ((c = io_getc(in)) != EOF)
	{
		if	(isspace(c))
			sb_putc(' ', buf);
		else	sb_putc(c, buf);

		if	(c == '|')
		{
			c = io_getc(in);

			if	(c != '|')
			{
				io_ungetc(c, in);
				break;
			}
			else	sb_putc(c, buf);
		}
	}
}

/*	Befehlsdefinitionen laden
*/

void DocTab_load (DocTab *tab, IO *io)
{
	int c, last, flag;
	char *p;
	StrBuf *desc;
	LoadFunc func;

	desc = sb_create(0);
	flag = 0;
	last = '\n';
	PushVarTab(RefVarTab(tab->var), NULL);

	while ((c = io_skipcom(io, NULL, last == '\n')) != EOF)
	{
		if	(c == '\\')
		{
			p = DocParseName(io, 0);

			if	(mstrcmp(PROTECT, p) == 0)
			{
				c = io_getc(io);

				if	(flag == 2)
					sb_putc('\n', desc);

				sb_putc(c, desc);
				flag = 1;
				last = c;
			}
			else if	(mstrcmp(BREAK, p) == 0)
			{
				break;
			}
			else if	((func = get_func(p)) != NULL)
			{
				func(tab, io, desc);
				sb_clean(desc);
				flag = 0;
				last = '\n';
			}
			else
			{
				if	(flag == 2)
					sb_putc('\n', desc);

				sb_putc('\\', desc);
				sb_puts(p, desc);
				sb_putc(';', desc);
				flag = 1;
			}
		}
		else if	(isspace(c))
		{
			if	(flag != 1)	continue;

			if	(last != '\n' || c != '\n')
			{
				sb_putc(c, desc);
				last = c;
			}
			else	flag = 2;
		}
		else
		{
			if	(flag == 2)
				sb_putc('\n', desc);

			sb_putc(c, desc);

			if	(c == '|')
				copy_verb(io, desc);

			flag = 1;
			last = c;
		}
	}

	PopVarTab();
	rd_deref(desc);
	io_close(io);
}

void DocTab_fload (DocTab *tab, const char *name)
{
	char *fname;

	if	(!tab || !name)
		return;

	fname = fsearch(DocPath, NULL, name, FILE_EXT);

	if	(!fname)
		fname = fsearch(CFGPATH, NULL, name, FILE_EXT);

	if	(fname)
	{
		DocTab_load(tab, io_lnum(io_fileopen(fname, "rd")));
		memfree(fname);
	}
	else	dbg_error(NULL, FMT, "m", mstrpaste(".", name, FILE_EXT));
}
