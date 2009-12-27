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


/*	Ladebefehle
*/

typedef void (*LoadFunc_t) (DocTab_t *tab, io_t *in, strbuf_t *desc);

static char *get_data (strbuf_t *buf)
{
	return buf->pos ? mstrncpy((char *) buf->data, buf->pos) : NULL;
}


static void cmd_eval (DocTab_t *tab, io_t *in, strbuf_t *desc)
{
	io_t *io = io_cmdpreproc(io_mstr(DocParseExpr(in)));
	CmdEvalFunc(io, NULL, 0);
	io_close(io);
}

static void cmd_load (DocTab_t *tab, io_t *in, strbuf_t *desc)
{
	char *p = DocParseLine(in, 0);
	DocTab_fload(tab, p);
	memfree(p);
}

static char *get_desc (strbuf_t *buf)
{
	io_t *io;
	strbuf_t *desc;
	int c;

	sb_setpos(buf, 0);
	io = langfilter(io_strbuf(buf), NULL);
	desc = new_strbuf(0);

	while ((c = io_getc(io)) != EOF)
		sb_putc(c, desc);

	io_close(io);
	return sb2str(desc);
}

void DocTab_def (DocTab_t *tab, io_t *in, strbuf_t *buf)
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
		io_message(in, MSG_DOC, 21, 0);
		memfree(DocParseLine(in, 0));
		break;
	}
}

static void cmd_show (DocTab_t *tab, io_t *in, strbuf_t *desc)
{
	reg_set(1, get_data(desc));
	io_psub(ioerr, "---- show\n$1****\n");
}

static struct {
	char *name;
	LoadFunc_t func;
} ctab[] = {
	{ "eval",	cmd_eval },
	{ "def",	DocTab_def },
	{ "include",	cmd_load },
	{ "show",	cmd_show },
};

static LoadFunc_t get_func (const char *name)
{
	int i;

	for (i = 0; i < tabsize(ctab); i++)
		if	(mstrcmp(ctab[i].name, name) == 0)
			return ctab[i].func;

	return NULL;
}


static void copy_verb (io_t *in, strbuf_t *buf)
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

void DocTab_load (DocTab_t *tab, io_t *io)
{
	int c, last, flag;
	char *p;
	strbuf_t *desc;
	LoadFunc_t func;

	desc = new_strbuf(0);
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
			else if	((func = get_func(p)) != NULL)
			{
				func(tab, io, desc);
				sb_clear(desc);
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
	del_strbuf(desc);
	io_close(io);
}

void DocTab_fload (DocTab_t *tab, const char *name)
{
	if	(tab && name)
		DocTab_load(tab, io_lnum(io_findopen(CFGPATH, name,
			FILE_EXT, "rzd")));
}
