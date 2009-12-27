/*
:*:create script documentation from comments
:de:Dokumentation aus Sourcefile generieren

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/pconfig.h>
#include <EFEU/strbuf.h>
#include <EFEU/Resource.h>
#include <EFEU/CmdPar.h>
#include <EFEU/Debug.h>
#include "DocBuf.h"
#include <ctype.h>

#define	BSIZE	4096

#define	E_CFG	"[efeuman:1]$!: syntax description not found.\n"

/*	Globale Variablen
*/

DocBuf doc_buf;	/* Dokumentbuffer */
int indent;	/* Einrücktiefe */

static int f_setpar (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *p = strchr(arg, '=');

	if	(!p)	return 0;

	*p = 0;
	var = CmdPar_var(cpar, arg, 1);
	*p = '=';

	memfree(var->value);
	var->value = mstrcpy(p + 1);
	return 0;
}

static CmdParEval setpar = {
	"setpar",
	"Parameterwert setzen",
	f_setpar,
};

static char *source = NULL;

static int f_setsrc (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	memfree(source);
	source = mstrcpy(arg);
	return 0;
}

static CmdParEval setsrc = {
	"setsrc",
	"Sourcefile mit Beschreibung",
	f_setsrc,
};

static int f_manpage (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	char *name;
	IO *def;
	IO *out;

	if	(arg && *arg == '@')
	{
		CmdPar_usage(cpar, iostd, arg);
		return 0;
	}

	name = CmdPar_psub(cpar, par, arg);
	out = io_fileopen(name, "w");
	memfree(name);

	if	(!(name = CmdPar_hpath(cpar)))
	{
		def = io_tmpbuf(BSIZE);
		io_puts("@head\n", def);

		if	(!sb_getpos(doc_buf.synopsis))
			sb_puts("@synopsis\n", doc_buf.tab[BUF_SYN]);

		if	(!sb_getpos(doc_buf.tab[BUF_DESC]))
		{
			sb_puts(GetResource("desc", NULL),
				doc_buf.tab[BUF_DESC]);
			sb_puts("\n@arglist\n", doc_buf.tab[BUF_DESC]);
		}

		DocBuf_write(&doc_buf, def);
		io_rewind(def);
	}
	else	def = io_fileopen(name, "r");

	CmdPar_iousage (cpar, out, def);
	io_close(def);
	io_close(out);
	return 0;
}

static CmdParEval eval_manpage = {
	"manpage",
	"Handbucheintrag generieren",
	f_manpage,
};

/*	Hilfsfunktionen
*/

static void srcline (IO *io, int c)
{
	while (c != EOF && c != '\n')
	{
		if	(c == '$')
			sb_putc(c, doc_buf.source);

		sb_putc(c, doc_buf.source);
		c = io_getc(io);
	}

	if	(sb_getpos(doc_buf.source))
		sb_putc('\n', doc_buf.source);
}

static char *buf2var (StrBuf *buf)
{
	IO *io;
	StrBuf *var;
	int c;

	var = sb_create(0);
	sb_setpos(buf, 0);
	io = langfilter(io_strbuf(buf), NULL);

	while ((c = io_getc(io)) != EOF)
		sb_putc(c, var);

	io_close(io);
	return sb2str(var);
}

static char *getpar (StrBuf *buf)
{
	int i, n, k;
	char *p;

	n = sb_size(buf);

	for (i = 1; i < n; i++)
	{
		if	(buf->data[i-1] == '\n' && buf->data[i] == '\n')
		{
			buf->pos = i;
			buf->nfree = buf->size - buf->pos;
			p = buf2var(buf);

			for (k = 0, i++; i < n; i++, k++)
				buf->data[k] = buf->data[i];

			buf->nfree = buf->size - k;
			buf->pos = 0;
			return p;
		}
	}

	p = buf2var(buf);
	sb_clean(buf);
	return p;
}


static void addline (IO *io, StrBuf *buf)
{
	int c;
	int pos;
	int flag;

	pos = 1;
	flag = 1;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')	break;

		if	(flag)
		{
			if	(c == ' ')	pos++;
			else if	(c == '\t')	pos = 8 + 8 * (pos / 8);
			else			flag = 0;

			if	(indent && pos > indent)
				flag = 0;

			if	(flag)	continue;
			if	(!indent)	indent = pos;
		} 
		else if	(indent > pos)	indent = pos;

		sb_putc(c, buf);
	}

	if	(flag && !doc_buf.var[VAR_TITLE])
	{
		doc_buf.var[VAR_TITLE] = buf2var(buf);
		sb_clean(buf);
	}
	else	sb_putc('\n', buf);
}

static int testkey (StrBuf *buf, const char *key)
{
	int c;

	for (; *key != 0; key++)
	{
		if	(*key != sb_getc(buf))
		{
			sb_setpos(buf, 0);
			return 0;
		}
	}

	do	c = sb_getc(buf);
	while	(c != EOF && c != '\n');

	return 1;
}

static void addbuf(StrBuf *buf)
{
	if	(sb_getpos(buf) == 0)	return;

	sb_setpos(buf, 0);

	if	(buf->data[0] == '$')
	{
		if	(testkey(buf, "$pconfig"))
		{
			IO *io = io_strbuf(buf);
			CmdPar_read(NULL, io, EOF, 0);
			io_close(io);
		}
		else	DocBuf_copy(&doc_buf, buf, NULL, NULL);
	}
	else if	(doc_buf.var[VAR_TITLE] == NULL)
	{
		doc_buf.var[VAR_TITLE] = getpar(buf);
		DocBuf_copy(&doc_buf, buf, NULL, NULL);
	}

	sb_clean(buf);
	sb_clean(doc_buf.source);
}

static void eval_script (IO *io, StrBuf *buf)
{
	int c;

	indent = 0;

	while ((c = io_getc(io)) != EOF)
	{
		switch (c)
		{
		case '#':
			switch (io_peek(io))
			{
			case '!':
			case '#':
				srcline(io, io_getc(io));
				break;
			default:
				addline(io, buf);
				break;
			}

			break;
		default:
			addbuf(buf);
			indent = 0;
			srcline(io, c);
			break;
		}
	}

	addbuf(buf);
}

static void eval_source (IO *io, StrBuf *buf)
{
	StrBuf *ptr;
	int at_start;
	int c;

	ptr = buf;
	at_start = 1;

	while ((c = io_skipcom(io, ptr, at_start)) != EOF)
	{
		if	(c == '\n')
		{
			addbuf(buf);
			at_start = 1;
			ptr = buf;
		}
		else
		{
			at_start = 0;
			ptr = NULL;
		}
	}

	addbuf(buf);
}

static void setval (CmdPar *par, const char *name, char *val)
{
	if	(val)
		CmdPar_setval(par, name, mstrcpy(val));
}

/*	Hauptprogramm
*/

int main (int argc, char **argv)
{
	CmdPar *par;
	StrBuf *buf;
	IO *io;
	VecBuf save;

	SetProgName(argv[0]);
	SetupStd();
	CmdParEval_add(&setpar);
	CmdParEval_add(&setsrc);

	par = CmdPar_ptr(NULL);
	CmdPar_load(par, par->name, 1);
	CmdPar_info(par, NULL);

	if	(CmdPar_eval(par, &argc, argv, 1) <= 0)
		exit(EXIT_FAILURE);

	DebugMode(GetResource("Debug", NULL));
	argc--;
	argv++;

	par = CmdPar_ptr(NULL);
	memcpy(&save, &par->var, sizeof(VecBuf));
	memset(&par->var, 0, sizeof(VecBuf));
	CmdPar_clean(par);
	memcpy(&par->var, &save, sizeof(VecBuf));
	CmdParEval_add(&eval_manpage);
	SetProgName(argv[0]);
	CmdPar_load(par, "help", 0);

	DocBuf_init(&doc_buf);
	doc_buf.var[VAR_NAME] = mstrcpy(par->name);

	if	(GetFlagResource("LoadConfig"))
	{
		CmdPar_load(par, par->name, 1);

		if	(CmdPar_eval(par, &argc, argv, 0) <= 0)
			exit(EXIT_FAILURE);

		CmdPar_usage(par, NULL, NULL);
		return 0;
	}

	buf = sb_create(BSIZE);
	io = NULL;

	if	(source)
	{
		io = io_lnum(io_fileopen(source, "rzd"));
		eval_source(io, buf);
	}
	else if ((io = io_fopen(argv[0], "r")))
	{
		io = io_lnum(io);
		eval_script(io, buf);
	}
	else if	(CmdPar_load(par, par->name, 1))
	{
		;
	}
	else	dbg_error(NULL, E_CFG, NULL);

	rd_deref(buf);
	io_close(io);

	setval(par, "Ident", doc_buf.var[VAR_TITLE]);
	setval(par, "Copyright", doc_buf.var[VAR_COPYRIGHT]);

	if	(CmdPar_eval(par, &argc, argv, 0) <= 0)
		exit(EXIT_FAILURE);

	CmdPar_usage(par, NULL, NULL);
	return 0;
}
