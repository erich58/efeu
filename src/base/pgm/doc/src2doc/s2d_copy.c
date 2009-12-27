/*
Kommentarbuffer umkopieren 

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

#include "src2doc.h"
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/efio.h>


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

static strbuf_t *set_var (SrcData_t *data, int par, io_t *io)
{
	char *p = io_mgets(io, "\n");
	memfree(data->var[par]);
	data->var[par] = parsub(p);
	memfree(p);
	return NULL;
}

static strbuf_t *get_buf (SrcData_t *data, int par, io_t *io)
{
	return data->tab[par];
}

static strbuf_t *get_synopsis (SrcData_t *data, int par, io_t *io)
{
	return data->synopsis;
}

static struct {
	char *key;
	strbuf_t *(*eval) (SrcData_t *data, int par, io_t *io);
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
};

static strbuf_t *eval_key (SrcData_t *data, const char *key, io_t *io)
{
	int i;

	for (i = 0; i < tabsize(evaltab); i++)
		if	(cmp(evaltab[i].key, key))
			return evaltab[i].eval(data, evaltab[i].par, io);

	return NULL;
}

/*	Stringbuffer umkopieren
*/

void SrcData_copy (SrcData_t *data, strbuf_t *buf)
{
	int need_nl;
	io_t *io;
	int c;

	if	(sb_getpos(data->buf) == 0)	return;

	need_nl = (buf && sb_getpos(buf));
	sb_begin(data->buf);
	io = langfilter(io_strbuf(data->buf), NULL);

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '$')
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
				buf = eval_key(data, p, io);
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
	sb_clear(data->buf);
}
