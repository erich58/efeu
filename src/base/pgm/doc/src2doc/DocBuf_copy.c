/*
Zwischenbuffer in Dokumentbuffer übertragen

$Copyright (C) 2000 Erich Frühstück
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

#include "DocBuf.h"
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/efio.h>
#include <EFEU/CmdPar.h>
#include <ctype.h>


static int cmp (const char *name, const char *arg)
{
	int flag = 0;

	while (*name && *name == tolower(*arg))
	{
		do
		{
			name++;

			if	(*name == '[')	flag = 1;
			else if	(*name != ']')	break;
		}
		while (*name);

		arg++;

		if	(*arg == 0 && (flag || *name == 0))	return 1;
	}

	return 0;
}

static strbuf_t *set_var (DocBuf_t *doc, int par, io_t *io, strbuf_t *buf)
{
	char *p = io_mgets(io, "\n");
	memfree(doc->var[par]);
	doc->var[par] = parsub(p);
	memfree(p);
	return NULL;
}

static strbuf_t *get_buf (DocBuf_t *doc, int par, io_t *io, strbuf_t *buf)
{
	return doc->tab[par];
}

static strbuf_t *get_synopsis (DocBuf_t *doc, int par, io_t *io, strbuf_t *buf)
{
	return doc->synopsis;
}

static strbuf_t *cpy_source (DocBuf_t *doc, int par, io_t *io, strbuf_t *buf)
{
	size_t n;
	char *p;

	if	(!buf)	return NULL;

	n = sb_getpos(doc->source);

	if	(n == 0)	return buf;

	p = (char *) doc->source->data;

	while (n && p[n - 1] == '\n')
		n--;

	sb_puts("---- verbatim\n", buf);

	for (; n-- > 0; p++)
		sb_putc(*p, buf);

	sb_puts("\n----\n", buf);
	return buf;
}

static struct {
	char *key;
	strbuf_t *(*eval) (DocBuf_t *doc, int par, io_t *io, strbuf_t *buf);
	int par;
} evaltab[] = {
	{ "head[er]",		set_var, VAR_HEAD },
	{ "name",		set_var, VAR_NAME },
	{ "title",		set_var, VAR_TITLE },
	{ "desc[ription]",	get_buf, BUF_DESC },
	{ "example[s]",		get_buf, BUF_EX },
	{ "see[also]",		get_buf, BUF_SEE },
	{ "diag[nostics]",	get_buf, BUF_DIAG },
	{ "note[s]",		get_buf, BUF_NOTE },
	{ "warn[ings]",		get_buf, BUF_WARN },
	{ "err[ors]",		get_buf, BUF_ERR },
	{ "copyright",		set_var, VAR_COPYRIGHT },
	{ "syn[opsis]",		get_synopsis, 0 },
	{ "source",		cpy_source, 0 },
};

static strbuf_t *eval_key (DocBuf_t *doc, const char *key,
	io_t *io, strbuf_t *buf)
{
	int i;

	for (i = 0; i < tabsize(evaltab); i++)
		if	(cmp(evaltab[i].key, key))
			return evaltab[i].eval(doc, evaltab[i].par, io, buf);

	return NULL;
}

void DocBuf_copy (DocBuf_t *doc, strbuf_t *base, strbuf_t *buf)
{
	int need_nl;
	io_t *io;
	int c;

	need_nl = (buf && sb_getpos(buf));
	sb_begin(base);
	io = langfilter(io_strbuf(base), NULL);

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\\')
		{
			c = io_getc(io);

			switch (c)
			{
			case '$':
			case '\\':
				break;
			default:
				io_ungetc(c, io);
				c = '\\';
				break;
			}
		}
		else if	(c == '$')
		{
			void *p;

			switch (io_scan(io, SCAN_INT|SCAN_NAME, &p))
			{
			case SCAN_INT:
				if	(buf)
					sb_puts(reg_get(*((int *) p)), buf);
				continue;
			case SCAN_NAME:

				do	c = io_getc(io);
				while	(isspace(c));

				io_ungetc(c, io);
				buf = eval_key(doc, p, io, buf);
				memfree(p);
				need_nl = (buf && sb_getpos(buf));
				continue;
			}
		}

		if	(buf)
		{
			if	(need_nl)
			{
				sb_putc('\n', buf);
				need_nl = 0;
			}

			sb_putc(c, buf);
		}
	}

	io_close(io);
	sb_clear(base);
}
