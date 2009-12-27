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

static char *set_args[2] = { NULL, NULL };

static StrBuf *set_var (DocBuf *doc, int par, IO *io, StrBuf *buf)
{
	char *p = io_mgets(io, "\n");
	memfree(doc->var[par]);
	doc->var[par] = mpsubvec(p, 2, set_args);
	memfree(p);
	return NULL;
}

static StrBuf *get_buf (DocBuf *doc, int par, IO *io, StrBuf *buf)
{
	return doc->tab[par];
}

static StrBuf *get_synopsis (DocBuf *doc, int par, IO *io, StrBuf *buf)
{
	return doc->synopsis;
}

static StrBuf *cpy_source (DocBuf *doc, int par, IO *io, StrBuf *buf)
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
	StrBuf *(*eval) (DocBuf *doc, int par, IO *io, StrBuf *buf);
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

static StrBuf *eval_key (DocBuf *doc, const char *key, IO *io, StrBuf *buf)
{
	int i;

	for (i = 0; i < tabsize(evaltab); i++)
		if	(cmp(evaltab[i].key, key))
			return evaltab[i].eval(doc, evaltab[i].par, io, buf);

	return NULL;
}

void DocBuf_copy (DocBuf *doc, StrBuf *base, StrBuf *buf, const char *name)
{
	int need_nl;
	IO *io;
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
			c = io_peek(io);

			if	(c == '1')
			{
				io_getc(io);
				sb_puts(name, buf);
				continue;
			}
			else if	(isalpha(c) || c == '_')
			{
				char *p = io_getname(io);

				do	c = io_getc(io);
				while	(isspace(c));

				io_ungetc(c, io);
				set_args[1] = (char *) name;
				buf = eval_key(doc, p, io, buf);
				memfree(p);
				need_nl = (buf && sb_getpos(buf));
				continue;
			}
			else	c = '$';
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
