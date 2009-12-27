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

StrBuf *parse_open (void)
{
	sb_clean(&parse_buf);
	return &parse_buf;
}

char *parse_close (StrBuf *buf)
{
	if	(sb_getpos(buf))
	{
		sb_putc(0, buf);
		return (char *) buf->data;
	}
	else	return NULL;
}

static void add_name (StrBuf *buf, IO *io, int c)
{
	while (isalnum(c) || c == '_')
	{
		sb_putc(c, buf);
		c = io_mgetc(io, 1);
	}

	io_ungetc(c, io);
}

static void add_block (StrBuf *buf, IO *io, int c, int flag)
{
	IO *aus = io_strbuf(buf);
	copy_block(io, aus, c, flag);
	io_close(aus);
}

/*	Namen abfragen
*/

char *parse_name (IO *io, int c)
{
	StrBuf *buf = parse_open();
	add_name(buf, io, c);
	return parse_close(buf);
}

/*	Block abfragen
*/

char *parse_block (IO *io, int beg, int end, int flag)
{
	StrBuf *buf = parse_open();
	sb_putc(beg, buf);
	add_block(buf, io, end, flag);
	return parse_close(buf);
}


/*	Deklaration parsen
*/

static void name_start (Decl *decl, StrBuf *buf)
{
	if	(decl->end < sb_getpos(buf))
		decl->start = sb_getpos(buf);
}

static void name_end (Decl *decl, StrBuf *buf)
{
	decl->end = sb_getpos(buf);
	decl->def = (char *) buf->data;
}

Decl *parse_decl (IO *io, int c)
{
	static Decl decl;
	StrBuf *buf;
	int n, flag, sflag, cflag;
	int argpos;

	memset(&decl, 0, sizeof(Decl));
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
			else if	(Decl_test(&decl, "return"))
				cflag = 1;
			else if	(Decl_test(&decl, "typedef"))
				decl.type = DECL_TYPE;
			else if	(Decl_test(&decl, "struct"))
				decl.type = DECL_STRUCT, sflag = 1;
			else if	(Decl_test(&decl, "enum"))
				decl.type = DECL_STRUCT, sflag = 1;
			else if	(Decl_test(&decl, "union"))
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
			add_block(buf, io, ']', 1);
			break;
		}
		else if	(c == '(')
		{
			if	(flag)
			{
				sb_putc(c, buf);
				add_block(buf, io, ')', 1);
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
					add_block(buf, io, ')', 1);

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
			add_block(buf, io, '}', 0),
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

	if	(!decl.type && decl.start)
		decl.type = DECL_VAR;

	if	(cflag)
	{
		switch (decl.type)
		{
		case DECL_VAR:	decl.type = DECL_SVAR; break;
		case DECL_FUNC:	decl.type = DECL_SFUNC; break;
		default:	decl.type = 0; break;
		}
	}
		
	while (buf->pos && isspace(buf->data[buf->pos - 1]))
	{
		buf->pos--;
		io_ungetc(buf->data[buf->pos], io);
	}

	decl.def = parse_close(buf);
	decl.arg = argpos ? decl.def + argpos : NULL;
	return &decl;
}

