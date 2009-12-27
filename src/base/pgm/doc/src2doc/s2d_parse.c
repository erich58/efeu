/*
Zerlegungshilfsfunktionen

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


static STRBUF(parse_buf, 1024);

strbuf_t *parse_open (void)
{
	sb_clear(&parse_buf);
	return &parse_buf;
}

char *parse_close (strbuf_t *buf)
{
	if	(sb_getpos(buf))
	{
		sb_putc(0, buf);
		return buf->data;
	}
	else	return NULL;
}

static void add_name (strbuf_t *buf, io_t *io, int c)
{
	while (isalnum(c) || c == '_')
	{
		sb_putc(c, buf);
		c = io_mgetc(io, 1);
	}

	io_ungetc(c, io);
}

static void add_block (strbuf_t *buf, io_t *io, int c)
{
	io_t *aus = io_strbuf(buf);
	copy_block(io, aus, c);
	io_close(aus);
}

/*	Namen abfragen
*/

char *parse_name (io_t *io, int c)
{
	strbuf_t *buf = parse_open();
	add_name(buf, io, c);
	return parse_close(buf);
}

/*	Block abfragen
*/

char *parse_block (io_t *io, int beg, int end)
{
	strbuf_t *buf = parse_open();
	sb_putc(beg, buf);
	add_block(buf, io, end);
	return parse_close(buf);
}


/*	Deklaration parsen
*/

static void name_start (Decl_t *decl, strbuf_t *buf)
{
	if	(decl->end < sb_getpos(buf))
		decl->start = sb_getpos(buf);
}

static void name_end (Decl_t *decl, strbuf_t *buf)
{
	decl->end = sb_getpos(buf);
	decl->def = (char *) buf->data;
}

Decl_t *parse_decl (io_t *io, int c)
{
	static Decl_t decl;
	strbuf_t *buf;
	int n, flag, sflag, cflag;
	int argpos;

	memset(&decl, 0, sizeof(Decl_t));
	buf = parse_open();
	n = 0;
	flag = sflag = cflag = 0;
	argpos = 0;

	while (c != EOF)
	{
		if	(c == ' ' || c == '\t')
		{
			sb_putc(c, buf);
			c = skip_blank(io);
			n++;
			continue;
		}
		else if	(isalpha(c) || c == '_')
		{
			name_start(&decl, buf);
			add_name(buf, io, c);
			name_end(&decl, buf);

			if	(Decl_test(&decl, "static"))
				cflag = 1;
			else if	(Decl_test(&decl, "typedef"))
				decl.type = DECL_TYPE;
			else if	(Decl_test(&decl, "struct"))
				decl.type = DECL_STRUCT, sflag = 1;
		}
		else if	(c == ':')
		{
			if	((c = io_mgetc(io, 1)) != ':')
			{
				io_ungetc(c, io);
				decl.type = DECL_LABEL;
				break;
			}
			else
			{
				name_start(&decl, buf);
				sb_puts("::", buf);
				name_end(&decl, buf);
			}
		}
		else if	(c == '[')
		{
			sb_putc(0, buf);
			argpos = sb_getpos(buf);
			sb_putc(c, buf);
			add_block(buf, io, ']');
			break;
		}
		else if	(c == '(')
		{
			if	(flag)
			{
				sb_putc(c, buf);
				add_block(buf, io, ')');
			}
			else
			{
				if	((c = skip_blank(io)) == '*')
				{
					sb_puts("(*", buf);
					flag = 1;
				}
				else
				{
					sb_putc(0, buf);
					argpos = sb_getpos(buf);
					sb_putc('(', buf);
					io_ungetc(c, io);
					add_block(buf, io, ')');

					if	(!decl.type)
						decl.type = decl.start ?
							DECL_FUNC : DECL_CALL;

					break;
				}
			}
		}
		else if	(flag && c == ')')
		{
			sb_putc(c, buf);
		}
		else if	(sflag && c == '{')
		{
			sb_putc(c, buf);
			add_block(buf, io, '}'),
			sflag = 0;
		}
		else if	(c == '*' || c == '&')
		{
			sb_putc(c, buf);
		}
		else
		{
			io_ungetc(c, io);
			break;
		}

		c = io_mgetc(io, 1);
	}

	if	(cflag)
		decl.type = 0;
	else if	(!decl.type && decl.start)
		decl.type = DECL_VAR;

	while (buf->pos && isspace(buf->data[buf->pos - 1]))
		buf->pos--;

	decl.def = parse_close(buf);
	decl.arg = argpos ? decl.def + argpos : NULL;
	return &decl;
}
