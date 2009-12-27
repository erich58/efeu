/*
:*:Create documentation from shell skript
:de:Dokumentation aus Shell-Skript generieren

$Copyright (C) 2004 Erich Frühstück
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
#include <EFEU/strbuf.h>
#include <EFEU/efio.h>
#include <EFEU/CmdPar.h>

#define	FKEY	"function"

static int indent = 0;

static void add_comment (IO *ein, StrBuf *buf)
{
	int c, n;
	int k, i;

	if	(buf->pos == 0)
		indent = 0;

	for (n = 1;;)
	{
		c = io_getc(ein);

		if	(c == ' ')	n++;
		else if	(c == '\t')	n = 8 * ((n + 8) / 8);
		else			break;
	}

	if	(c == '\n')
	{
		sb_putc(c, buf);
		indent = 0;
		return;
	}

	if	(indent == 0)
	{
		indent = n;
	}
	else if	(n > indent)
	{
		k = n / 8;

		for (i = 0; i < k; i++)
			sb_putc('\t', buf);

		k = n % 8;

		for (i = 0; i < k; i++)
			sb_putc(' ', buf);
	}

	while (c != '\n' && c != EOF)
	{
		sb_putc(c, buf);
		c = io_getc(ein);
	}

	sb_putc(c, buf);
}

static void insert_code (DocBuf *doc)
{
	StrBuf *buf = doc->tab[BUF_DESC];

	if	(buf->pos && buf->data[buf->pos - 1] != '\n')
		sb_putc('\n', buf);

	sb_puts("---- verbatim\n", buf);
	sb_putc(0, doc->source);
	sb_puts((char *) doc->source->data, buf);
	sb_puts("\n----\n", buf);
}

static void add_code (SrcData *data, int c)
{
	while (c != EOF)
	{
		if	(c == '\\')
		{
			sb_putc(c, data->doc.source);
			c = io_getc(data->ein);

			if	(c == EOF)	break;
		}
		else if	(c == '\n' && io_peek(data->ein) == '#')
		{
			break;
		}

		sb_putc(c, data->doc.source);
		c = io_getc(data->ein);
	}
}

static void sh_eval (SrcData *data, const char *name)
{
	int at_top, new_par, c;

	at_top = 1;
	new_par = 0;

	while ((c = io_getc(data->ein)) != EOF)
	{
		if	(c == '#')
		{
			c = io_peek(data->ein);

			if	(c == '!' || c == '#')
			{
				do	c = io_getc(data->ein);
				while	(c != EOF && c != '\n');

				continue;
			}

			add_comment(data->ein, &data->buf);
			new_par = 1;
		}
		else if	(at_top)
		{
			SrcData_title(data, name);
			at_top = 0;
		}
		else if	(new_par)
		{
			char *p;

			while (isspace(c))
				c = io_getc(data->ein);

			if	(c == '#')
			{
				SrcData_copy(data, NULL, name);
				add_comment(data->ein, &data->buf);
				continue;
			}

			sb_clean(data->doc.source);
			p = parse_name(data->ein, c);
			sb_puts(p, data->doc.source);
			c = io_getc(data->ein);

			while (c == ' ' || c == '\t')
			{
				sb_putc(c, data->doc.source);
				c = io_getc(data->ein);
			}

			if	(mstrcmp(p, FKEY) == 0)
			{
				p = parse_name(data->ein, c);
				sb_puts(p, data->doc.source);
				c = io_getc(data->ein);
			}
			else if	(c != '(')
			{
				memfree(p);
				p = NULL;
			}

			add_code(data, c);
			SrcData_copy(data, NULL, p ? p : name);
			new_par = 0;

			if	(InsertCode)
				insert_code(&data->doc);
		}
		else	new_par = 0;
	}

	SrcData_copy(data, NULL, name);
}

void s2d_sh (const char *name, IO *ein, IO *aus)
{
	SrcData data;

	SrcData_init(&data, ein);
	data.doc.var[VAR_NAME] = mstrcpy(name);
	data.ppdef = NULL;
	data.ppdim = 0;
	data.mask = 0;

	io_xprintf(aus, "\\mpage[%s] %s\n", Secnum ? Secnum : "7",
		data.doc.var[VAR_NAME]);

	sh_eval(&data, name);
	SrcData_write(&data, aus);
}
