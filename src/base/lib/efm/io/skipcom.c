/*
Kommentare lesen/zwischenspeichern

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/io.h>

#define	PROMPT	"/* "	/**/

static void comment_line (IO *io, StrBuf *buf, int flag)
{
	int c, escape;

	do	c = io_getc(io);
	while	(c == ' ' || c == '\t');

	escape = 0;

	while (escape || c != '\n')
	{
		if	(c == EOF)	return;
		if	(c == '\n')	io_linemark(io);

		if	(buf)	sb_putc(c, buf);

		escape = (c == '\\') ? !escape : 0;
		c = io_getc(io);
	}
	
	if	(buf)	sb_putc(c, buf);

	if	(!flag)
		io_ungetc(c, io);
}

static void comment_cstyle (IO *io, StrBuf *buf, int flag)
{
	int indent, c, depth;
	char *prompt;

	if	(buf)
		sb_clean(buf);

	prompt = io_prompt(io, PROMPT);
	c = io_getc(io);

	for (depth = 0; c == '\t'; depth++)
		c = io_getc(io);

	while (c == '\n' && depth == 0)
		c = io_getc(io);

	indent = 0;

	while (c != EOF)
	{
		if	(c == '*')
		{
			if	(io_peek(io) == '/')
			{
				io_getc(io);
				break;
			}
		}

		if	(c == '\t' && indent)
		{
			indent--;
		}
		else
		{
			if	(c == '\n')
			{
				indent = depth;
			}
			else if	(indent)
			{
				depth -= indent;
				indent = 0;
			}

			if	(buf)
				sb_putc(c, buf);
		}

		c = io_getc(io);
	}

	io_prompt(io, prompt);

	if	(flag && io_peek(io) == '\n')
		io_getc(io);
}

/*
Die Funktion |$1| liest das nächste Zeichen aus der Eingabestruktur <io>,
wobei Kommentare im C++-Style überlesen werden.

Falls <buf> verschieden
von NULL ist, wird der Kommentartext in <buf> gespeichert.
Ein Kommentar der Form <|/\* ... *\/|> überschreibt den Buffer,
während ein Kommentar der Form <*\// ... <newline>*> am Ende des
Buffers angehängt wird.

Falls <flag> verschieden von 0 ist,
wird ein dem Kommentar folgender Zeilenvorschub, bzw. der das Kommentar
abschließende Zeilenvorschub überlesen.
*/

int io_skipcom (IO *io, StrBuf *buf, int flag)
{
	int c;

	while ((c = io_getc(io)) == '/')
	{
		switch ((c = io_getc(io)))
		{
		case '/':	comment_line(io, buf, flag); break;
		case '*':	comment_cstyle(io, buf, flag); break;
		default:	io_ungetc(c, io); /* FALLTROUGH */
		case EOF:	return '/';
		}
	}

	return c;
}

/*
$SeeAlso
\mref{io_getc(3)}.
*/
