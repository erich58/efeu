/*
Bereich einlesen

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

static void cp_verb (IO *in, StrBuf *buf)
{
	int c;
	
	sb_put('|', buf);

	while ((c = io_getc(in)) != EOF)
	{
		sb_put(c, buf);

		if	(c == '|')
		{
			c = io_getc(in);

			if	(c != '|')
			{
				io_ungetc(c, in);
				break;
			}
			else	sb_put(c, buf);
		}
	}
}

static int cp_minus (IO *in, StrBuf *buf)
{
	int n, c;

	sb_put('-', buf);

	for (n = 1; (c = io_getc(in)) == '-'; n++)
		sb_put('-', buf);

	io_ungetc(c, in);
	return n;
}

static void cp_line (IO *in, StrBuf *buf)
{
	int c;

	while ((c = io_getc(in)) != EOF)
	{
		sb_put(c, buf);

		if	(c == '\n')	break;
	}
}

static void cp_reg (IO *in, StrBuf *buf)
{
	int c;

	do
	{
		cp_line(in, buf);
		c = io_getc(in);

		if	(c == '-' && cp_minus(in, buf) > 3)
		{
			cp_line(in, buf);
			return;
		}
		else	sb_put(c, buf);
	}
	while	(c != EOF);
}

static int test_key (const char *key, const char *name)
{
	if	(key == NULL)	return 0;

	while (*key != 0)
	{
		if	(*key == '*')	return 1;
		if	(*key != *name)	return 0;

		key++;
		name++;
	}

	return (*name == 0);
}

char *DocParseBlock (IO *in, int mode, const char *beg,
	const char *end, const char *toggle)
{
	int nl, c, depth;
	StrBuf *buf;
	char *p;

	buf = (mode || toggle) ? sb_acquire() : NULL;
	nl = 1;
	depth = 0;

	while ((c = io_skipcom(in, NULL, nl)) != EOF)
	{
		if	(c == '\\')
		{
			c = io_getc(in);

			if	(isalpha(c) || c == '_')
			{
				io_ungetc(c, in);
				p = DocParseName(in, 0);

				if	(test_key(end, p))
				{
					if	(depth == 0)
					{
						DocSkipSpace(in, 1);
						break;
					}
					else	depth--;
				}
				else if	(test_key(beg, p))
				{
					depth++;
				}
				else if	(depth == 0 && test_key(toggle, p))
				{
					DocSkipSpace(in, 1);
					toggle = NULL;
					mode = !mode;
					continue;
				}

				if	(mode)
				{
					sb_putc('\\', buf);
					sb_puts(p, buf);
					sb_putc(';', buf);
				}
			}
			else if	(mode)	
			{
				sb_putc('\\', buf);
				sb_putc(c, buf);
			}
		}
		else if	(c == '|')
		{
			cp_verb(in, mode ? buf : NULL);
			nl = 0;
		}
		else if	(nl && c == '-')
		{
			if	(cp_minus(in, mode ? buf : NULL) > 3)
			{
				cp_reg(in, mode ? buf : NULL);
				nl = 1;
			}
			else	nl = 0;
		}
		else
		{
			if	(mode)	sb_putc(c, buf);

			nl = (c == '\n');
		}
	}

	return sb_cpyrelease(buf);
}
