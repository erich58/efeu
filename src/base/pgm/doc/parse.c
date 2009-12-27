/*
Lesefunktionen

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

/*	Kopierbuffer
*/

static STRBUF(parse_buf, 1024);

static char *parse_data (void)
{
	if	(parse_buf.pos)
	{
		sb_putc(0, &parse_buf);
		return sb_memcpy(&parse_buf);
	}
	else	return NULL;
}


/*	Anführungen
*/

static void copy_quote (io_t *in, strbuf_t *buf, int key)
{
	int c;

	while ((c = io_getc(in)) != EOF)
	{
		if	(c == key)
		{
			if	(io_peek(in) != key)
				break;

			c = io_getc(in);
		}

		sb_putc(c, buf);
	}
}

static void test_quote (io_t *in, strbuf_t *buf, int key)
{
	if	(io_peek(in) == key)
	{
		io_getc(in);
		sb_putc(key, buf);
	}
	else	copy_quote(in, buf, key);
}

static char *quote_arg (io_t *in, int c)
{
	sb_clear(&parse_buf);
	copy_quote(in, &parse_buf, c);
	sb_putc(0, &parse_buf);
	return sb_memcpy(&parse_buf);
}


/*	Argument mit Testfunktion
*/

static char *parse_arg (io_t *in, strbuf_t *buf, int (*test)(int c))
{
	int c;

	sb_clear(&parse_buf);

	while ((c = io_skipcom(in, NULL, 0)) != EOF)
	{
		if	(test(c))
		{
			io_ungetc(c, in);
			break;
		}
		else if	(c == '\'')
		{
			test_quote(in, &parse_buf, c);
		}
		else	sb_putc(c, &parse_buf);
	}

	sb_putc(0, &parse_buf);
	return sb_memcpy(&parse_buf);
}

/*	Makroargument
*/

static int macarg_end (int c) { return (c == ',' || c == ')'); }

char *DocParseMacArg (io_t *in)
{
	return parse_arg(in, &parse_buf, macarg_end);
}


/*	Rest der Zeile lesen
*/

char *DocParseLine (io_t *in, int flag)
{
	int c, d;

	sb_clear(&parse_buf);

	while ((c = io_skipcom(in, NULL, 0)) != EOF)
	{
		if	(c == '\n')
		{
			if	(!flag)	break;

			d = io_peek(in);

			if	(d != ' ' && d != '\t')
				break;

			DocSkipSpace(in, 0);
		}

		sb_putc(c, &parse_buf);
	}

	return parse_data();
}


/*	Absatz lesen
*/

char *DocParsePar (io_t *in)
{
	int c, flag;

	sb_clear(&parse_buf);
	flag = 1;

	while ((c = io_skipcom(in, NULL, flag)) != EOF)
	{
		if	(c == '\n')
		{
			if	(flag)
			{
				io_ungetc(c, in);
				break;
			}

			flag = 1;
		}
		else	flag = 0;

		sb_putc(c, &parse_buf);
	}

	return parse_data();
}


/*	Block Lesen: Einzelne Zeile oder geklammert
*/

static void copy_str (io_t *in, strbuf_t *buf, int end)
{
	int c, escape;
	
	escape = 0;

	while ((c = io_getc(in)) != EOF)
	{
		if	(escape)	escape = 0;
		else if	(end == c)	return;
		else			escape = (c == '\\');

		sb_putc(c, buf);
	}
}

static void copy_brace (io_t *in, strbuf_t *buf)
{
	int c = 0;
	int depth = 0;

	while ((c = io_skipcom(in, NULL, c == '\n')) != EOF)
	{
		switch (c)
		{
		case '{':
			depth++;
			break;
		case '}':
			if	(depth <= 0)	return;

			depth--;
			break;
		case '"':
		case '\'':
			sb_putc(c, buf);
			copy_str(in, buf, c);
			break;
		default:
			break;
		}

		sb_putc(c, buf);
	}
}


char *DocParseExpr (io_t *in)
{
	int c;

	do	c = io_skipcom(in, NULL, 0);
	while	(c == ' ' || c == '\t');

	switch (c)
	{
	case EOF:
		return NULL;
	case '{':
		c = DocSkipWhite(in);

		if	(c == '}')	return NULL;

		sb_clear(&parse_buf);
		copy_brace(in, &parse_buf);
		
		do	c = io_getc(in);
		while	(c == ' ' || c == '\t');

		if	(c != '\n')
			io_ungetc(c, in);

		return parse_data();
	case '\n':
		return DocParsePar(in);
	default:
		io_ungetc(c, in);
		return DocParseLine(in, 1);
	}
}


/*	Argument lesen
*/

static int word_end (int c) { return isspace(c); }

static char *do_parse (io_t *in, int beg, int end)
{
	int c, depth;

	if	(beg == 0)
	{
		beg = io_skipcom(in, NULL, 0);

		switch (beg)
		{
		case '{':
			end = '}';
			break;
		case '\'':
		case '"':
			return quote_arg(in, beg);
		case '\n':
			return DocParsePar(in);
		default:
			io_ungetc(beg, in);

			if	(end == 0)
				return parse_arg(in, &parse_buf, word_end);

			beg = EOF;
			break;
		}
	}
	else if	(beg != EOF)
	{
		c = io_skipcom(in, NULL, 0);

		if	(c != beg)
		{
			io_ungetc(c, in);
			return NULL;
		}

		if	(beg == end)
			return quote_arg(in, beg);
	}

	sb_clear(&parse_buf);
	depth = 0;

	while ((c = io_skipcom(in, NULL, 0)) != EOF)
	{
		/*
		if	(c == '\\')
		{
			c = io_getc(in);

			if	(c != '\\' && c != '\'' && c != beg && c != end)
				sb_putc('\\', &parse_buf);
		}
		else
		*/
		if	(c == '\'')
		{
			test_quote(in, &parse_buf, c);
			continue;
		}
		else if	(c == end)
		{
			if	(depth <= 0)	break;
			else			depth--;
		}
		else if	(c == beg)	depth++;

		sb_putc(c, &parse_buf);
	}

	sb_putc(0, &parse_buf);
	return sb_memcpy(&parse_buf);
}

char *DocParseArg (io_t *in, int beg, int end, int flag)
{
	char *p = do_parse(in, beg, end);

	if	(flag)
	{
		int c;

		do	c = io_skipcom(in, NULL, 0);
		while	(c == ' ' || c == '\t');

		io_ungetc(c, in);
	}

	return p;
}


/*	Namen einlesen: Da diese nur temporär zur Suche verwendet
	werden, liefert die Funktion den Buffer anstelle einer Kopie.
*/

char *DocParseName (io_t *in, int key)
{
	int c;

	c = io_peek(in);

	if	(!isalpha(c) && c != '_')
		return key ? msprintf("%c", key) : NULL;

	sb_clear(&parse_buf);

	if	(key)	sb_putc(key, &parse_buf);

	while ((c = io_getc(in)) != EOF)
	{
		if	(isalnum(c) || c == '_')
			sb_putc(c, &parse_buf);
		else	break;
	}

	switch (c)
	{
	case ' ':
	case '\t':
		DocSkipSpace(in, 0);
		break;
	case ';':
		break;
	default:
		io_ungetc(c, in);
		break;
	}

	sb_putc(0, &parse_buf);
	return parse_buf.data;
}

/*	Ganzzahlwert ohne Vorzeichen einlesen.
*/

int DocParseNum (io_t *in)
{
	int c;

	sb_clear(&parse_buf);

	while ((c = io_getc(in)) != EOF)
	{
		if	(!isdigit(c))
		{
			io_ungetc(c, in);
			break;
		}
		else	sb_putc(c, &parse_buf);
	}

	if	(parse_buf.pos)
	{
		sb_putc(0, &parse_buf);
		return atoi((char *) parse_buf.data);
	}
	else	return 0;
}


/*	Steuerflags einlesen: Diese werden nur temporär verwendet
*/

char *DocParseFlags (io_t *in)
{
	int c;

	sb_clear(&parse_buf);

	while ((c = io_getc(in)) != EOF)
	{
		if	(isspace(c))
		{
			io_ungetc(c, in);
			DocSkipSpace(in, 0);
			break;
		}
		else if	(c == ';')
		{
			break;
		}
		else	sb_putc(c, &parse_buf);
	}

	if	(parse_buf.pos)
	{
		sb_putc(0, &parse_buf);
		return (char *) parse_buf.data;
	}

	return NULL;
}
