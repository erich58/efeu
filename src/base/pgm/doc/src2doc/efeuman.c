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

/*	Globale Variablen
*/

DocBuf_t DocBuf;	/* Dokumentbuffer */
int indent;		/* Einrücktiefe */

static int f_setpar (CmdPar_t *cpar, CmdParVar_t *var,
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

static CmdParEval_t eval_setpar = {
	"setpar",
	"Parameterwert setzen",
	f_setpar,
};

static int f_manpage (CmdPar_t *cpar, CmdParVar_t *var,
	const char *par, const char *arg)
{
	char *name;
	io_t *def;
	io_t *out;

	if	(arg && *arg == '@')
	{
		CmdPar_usage(cpar, iostd, arg);
		return 0;
	}

	name = CmdPar_psub(cpar, par, arg);
	out = io_fileopen(name, "w");
	memfree(name);

	def = io_tmpbuf(BSIZE);
	io_puts("@head\n", def);

	if	(!sb_getpos(DocBuf.synopsis))
		sb_puts("@synopsis\n", DocBuf.tab[BUF_SYN]);

	if	(!sb_getpos(DocBuf.tab[BUF_DESC]))
	{
		sb_puts(GetResource("desc", NULL), DocBuf.tab[BUF_DESC]);
		sb_puts("\n@arglist\n", DocBuf.tab[BUF_DESC]);
	}

	DocBuf_write(&DocBuf, def);
	io_rewind(def);
	CmdPar_iousage (cpar, out, def);
	io_close(def);

	io_close(out);
	return 0;
}

static CmdParEval_t eval_manpage = {
	"manpage",
	"Handbucheintrag generieren",
	f_manpage,
};

/*	Hilfsfunktionen
*/

static void srcline (io_t *io, int c)
{
	while (c != EOF && c != '\n')
	{
		if	(c == '$')
			sb_putc(c, DocBuf.source);

		sb_putc(c, DocBuf.source);
		c = io_getc(io);
	}

	if	(sb_getpos(DocBuf.source))
		sb_putc('\n', DocBuf.source);
}

static char *buf2var (strbuf_t *buf)
{
	io_t *io;
	strbuf_t *var;
	int c;

	var = new_strbuf(0);
	sb_setpos(buf, 0);
	io = langfilter(io_strbuf(buf), NULL);

	while ((c = io_getc(io)) != EOF)
		sb_putc(c, var);

	io_close(io);
	return sb2str(var);
}

static char *getpar (strbuf_t *buf)
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
	sb_clear(buf);
	return p;
}


static void addline (io_t *io, strbuf_t *buf)
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

	if	(flag && !DocBuf.var[VAR_TITLE])
	{
		DocBuf.var[VAR_TITLE] = buf2var(buf);
		sb_clear(buf);
	}
	else	sb_putc('\n', buf);
}

static int testkey (strbuf_t *buf, const char *key)
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

static void addbuf(strbuf_t *buf)
{
	if	(sb_getpos(buf) == 0)	return;

	sb_setpos(buf, 0);

	if	(buf->data[0] == '$')
	{
		if	(testkey(buf, "$pconfig"))
		{
			io_t *io = io_strbuf(buf);
			CmdPar_read(NULL, io, EOF, 0);
			io_close(io);
		}
		else	DocBuf_copy(&DocBuf, buf, NULL);
	}
	else if	(DocBuf.var[VAR_TITLE] == NULL)
	{
		DocBuf.var[VAR_TITLE] = getpar(buf);
		DocBuf_copy(&DocBuf, buf, NULL);
	}

	sb_clear(buf);
	sb_clear(DocBuf.source);
}

static void eval_script (io_t *io, strbuf_t *buf)
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

static void eval_source (io_t *io, strbuf_t *buf)
{
	strbuf_t *ptr;
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

/*	Hauptprogramm
*/

int main (int argc, char **argv)
{
	CmdPar_t *par;
	strbuf_t *buf;
	io_t *io;
	vecbuf_t save;

	SetProgName(argv[0]);
	SetupStd();
	CmdParEval_add(&eval_setpar);

	par = CmdPar_ptr(NULL);
	CmdPar_load(par, par->name, 1);
	CmdPar_info(par, NULL);

	if	(CmdPar_eval(par, &argc, argv, 1) <= 0)
		exit(EXIT_FAILURE);

	DebugMode(GetResource("Debug", NULL));
	argc--;
	argv++;

	par = CmdPar_ptr(NULL);
	memcpy(&save, &par->var, sizeof(vecbuf_t));
	memset(&par->var, 0, sizeof(vecbuf_t));
	CmdPar_clean(par);
	memcpy(&par->var, &save, sizeof(vecbuf_t));
	CmdParEval_add(&eval_manpage);
	SetProgName(argv[0]);
	CmdPar_load(par, "help", 0);

	DocBuf_init(&DocBuf);
	DocBuf.var[VAR_NAME] = mstrcpy(par->name);

	io = io_lnum(io_fileopen(argv[0], "rzd"));
	buf = new_strbuf(BSIZE);

	if	(patcmp("*.c", argv[0], NULL))
	{
		eval_source(io, buf);
	}
	else	eval_script(io, buf);

	del_strbuf(buf);
	io_close(io);

	if	(CmdPar_eval(par, &argc, argv, 0) <= 0)
		exit(EXIT_FAILURE);

	CmdPar_usage(par, NULL, NULL);
	return 0;
}
