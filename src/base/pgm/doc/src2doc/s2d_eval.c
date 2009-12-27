/*
Auswertungsfunktion für Sourcefiles

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
#include <EFEU/strbuf.h>
#include <EFEU/efio.h>

static void subcopy (IO *ein, IO *aus, int c, int flag);
static void copy_str (IO *ein, IO *aus, int delim);


static void copy_str (IO *ein, IO *aus, int delim)
{
	int c, flag;

	flag = 0;
	io_protect(ein, 1);

	while ((c = io_getc(ein)) != EOF)
	{
		io_putc(c, aus);

		if	(flag)		flag = 0;
		else if	(c == delim)	break;
		else if	(c == '\\')	flag = 1;
	}

	io_protect(ein, 0);
}

static void subcopy (IO *ein, IO *aus, int c, int flag)
{
	if	(!(flag && isspace(c)))
		io_putc(c, aus);

	switch (c)
	{
	case '"':
	case '\'':	copy_str(ein, aus, c); break;
	case '{':	copy_block(ein, aus, '}', 0); break;
	case '(':	copy_block(ein, aus, ')', 1); break;
	case '[':	copy_block(ein, aus, ']', 1); break;
	default:	break;
	}
}

void copy_block (IO *ein, IO *aus, int end, int flag)
{
	char *prompt;
	int c, last;

	prompt = io_prompt(ein, ">>> ");
	last = 0;

	while ((c = io_getc(ein)) != EOF && c != end)
	{
		subcopy(ein, aus, c, (flag && last == '\n'));
		last = c;
	}

	io_putc(end, aus);
	io_prompt(ein, prompt);
}


static void skipline (SrcData *data, int c)
{
	SrcData_copy(data, NULL, NULL);

	do	subcopy(data->ein, NULL, c, 0);
	while ((c = io_skipcom(data->ein, NULL, 0)) != EOF && c != '\n');
}

static void srcline (SrcData *data, char *head, int c)
{
	StrBuf *buf = data->doc.tab[BUF_DESC];
	IO *aus = io_strbuf(buf);

	if	(sb_getpos(data->buf))
		DocBuf_copy(&data->doc, data->buf, buf, NULL);

	if	(buf->pos && buf->data[buf->pos - 1] != '\n')
		sb_putc('\n', buf);

	sb_puts("---- verbatim\n", buf);
	sb_puts(head, buf);

	do	subcopy(data->ein, aus, c, 0);
	while ((c = io_skipcom(data->ein, NULL, 0)) != EOF && c != '\n');

	io_close(aus);
	sb_puts("\n----\n", buf);
}

/*	Präprozessorzeile umkopieren
*/

void ppcopy (IO *ein, StrBuf *val, StrBuf *com)
{
	int c;

	while ((c = io_skipcom(ein, com, 0)) != EOF)
	{
		if	(c == '\n')	break;
		if	(c == '\\')	c = io_getc(ein);

		if	(val)	sb_putc(c, val);
	}
}


static void ppline (SrcData *data)
{
	char *name;
	int n;

	name = parse_name(data->ein, skip_blank(data->ein));

	for (n = 0; n < data->ppdim; n++)
	{
		if	(mstrcmp(name, data->ppdef[n].name) == 0)
		{
			data->ppdef[n].eval(data, data->ppdef[n].name);
			return;
		}
	}

	SrcData_copy(data, NULL, NULL);
	ppcopy(data->ein, NULL, NULL);
}

static void subcopy2 (IO *ein, IO *aus, int delim)
{
	int c;

	while ((c = io_skipcom(ein, NULL, 0)) != EOF && c != delim)
		subcopy(ein, aus, c, 0);

	io_ungetc(c, ein);
}

static void keyline (SrcData *data, int c)
{
	Decl *decl;
	IO *aus;
	char *name;
	decl = parse_decl(data->ein, c);

	name = mstrncpy(decl->def + decl->start, decl->end - decl->start);

	if	(InsertCode)
	{
		StrBuf *buf = data->doc.tab[BUF_DESC];

		if	(decl->type & data->mask)
		{
			aus = io_strbuf(data->doc.synopsis);
			Decl_print(decl, aus, name);
			io_close(aus);
		}

		sb_printf(data->doc.tab[BUF_DESC], "\n/* %s */\n", name);
		SrcData_copy(data, buf, name);
		aus = io_strbuf(buf);
		io_puts("---- verbatim\n", aus);
		io_puts(decl->def, aus);
		io_puts(decl->arg, aus);

		switch (decl->type)
		{
		case DECL_VAR:
		case DECL_SVAR:
		case DECL_TYPE:
		case DECL_STRUCT:
			subcopy2(data->ein, aus, ';');
			break;
		default:
			break;
		}

		subcopy2(data->ein, aus, '\n');

		switch (decl->type)
		{
		case DECL_FUNC:
		case DECL_SFUNC:

			while ((c = io_skipcom(data->ein, NULL, 0)) == '\n')
				subcopy(data->ein, aus, c, 0);

			if	(c != EOF)
			{
				subcopy(data->ein, aus, c, 0);
				subcopy2(data->ein, aus, '\n');
			}

			break;
		default:
			break;
		}

		sb_puts("\n----\n", buf);
		io_close(aus);
		memfree(name);
		return;
	}

	if	(decl->type & data->mask)
	{
		aus = io_strbuf(data->doc.synopsis);
		Decl_print(decl, aus, name);
		io_close(aus);

		sb_printf(data->doc.tab[BUF_DESC], "\n/* %s */\n", name);
		SrcData_copy(data, data->doc.tab[BUF_DESC], name);
	}
	else if	(sb_getpos(data->buf) && (decl->type & data->xmask))
	{
		sb_printf(data->doc.tab[BUF_DESC], "\n/* %s */\n", name);
		SrcData_copy(data, data->doc.tab[BUF_DESC], name);

		aus = io_strbuf(data->doc.tab[BUF_DESC]);
		io_puts("\n\\code\n", aus);
		Decl_print(decl, aus, name);
		io_puts("\\end\n", aus);
		io_close(aus);
	}
	else	SrcData_copy(data, NULL, NULL);

	while ((c = io_skipcom(data->ein, NULL, 0)) != EOF && c != '\n')
		subcopy(data->ein, NULL, c, 0);

	memfree(name);
}


static char *get_title (StrBuf *buf)
{
	char *p;
	size_t n;

	sb_putc(0, buf);
	p = (char *) buf->data;

	for (n = 0; p[n] != 0; n++)
	{
		if	(strncmp("(c)", p + n, 3) == 0)
		{
			sb_setpos(buf, n);
			break;
		}
	}

	return sb2str(buf);
}

void SrcData_title (SrcData *data, const char *name)
{
	if	(sb_getpos(data->buf))
	{
		StrBuf *buf = sb_create(0);
		SrcData_copy(data, buf, name);
		memfree(data->doc.var[VAR_TITLE]);
		data->doc.var[VAR_TITLE] = get_title(buf);
	}
}

void SrcData_eval (SrcData *data, const char *name)
{
	int c;

	io_ungetc(skip_space(data->ein, data->buf), data->ein);

	if	(sb_getpos(data->buf))
	{
		StrBuf *buf = sb_create(0);
		SrcData_copy(data, buf, name);
		memfree(data->doc.var[VAR_TITLE]);
		data->doc.var[VAR_TITLE] = get_title(buf);
	}

	while ((c = skip_space(data->ein, data->buf)) != EOF)
	{
		if	(c == '#')
		{
			ppline(data);
		}
		else if	(isalpha(c) || c == '_')
		{
			keyline(data, c);
		}
		else if	(InsertCode)
		{
			srcline(data, NULL, c);
		}
		else	skipline(data, c);
	}

	SrcData_copy(data, NULL, name);
}
