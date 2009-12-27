/*
Kommentare lesen/zwischenspeichern

$Copyright (C) 1999, 2008 Erich Frühstück
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

static int indent;	/* Einrücktiefe */

static void skip_cstyle (IO *io);
static int skip_line (IO *io);

static void save_cstyle (IO *io, StrBuf *buf, int flag);
static int save_line (IO *io, StrBuf *buf);

/*
Die Funktion |$1| liest das nächste Zeichen aus der Eingabestruktur <io>,
wobei Kommentare im C99/C++-Style überlesen werden.
Falls <flag> verschieden von 0 ist,
wird ein dem Kommentar folgender Zeilenvorschub, bzw. der das Kommentar
abschließende Zeilenvorschub überlesen.
Die übliche Anwendung ist das Setzen von <flag> am Anfang einer Zeile.

Kommentare können für Dokumentationszwecke ausgewertet werden.
Falls <buf> verschieden von NULL ist,
wird der Kommentartext in <buf> gespeichert.
Um unterschiedliche Kommentierungsstile zu vereinheitlichen, werden
eine Reihe von Bereinigungen am Kommentartext vorgenommen.

Für Kommentare der Form |/\*| ... |*\/| gelten folgende Regeln:
*	Beliebig viele Sterne am Anfang und Ende des Kommentars
	werden entfernt, So speichert z.B. das Kommentar |/\*****A*****\/|
	nur den Buchstaben |A|.
*	Ein Backslash an Anfang oder Ende des Kommentars wird entfernt.
	Diese Regel ermöglicht Sterne am Anfang und Ende des Kommentars.
*	Die Zeichenfolge "Leerzeichen Stern" am Anfang einer Zeile wird
	ignoriert.

Bei Kommentaren der Form |//| ... \<newline\> werden
aufeinanderfolgende Kommentare, die jeweils am Zeilenanfang beginnen,
zusammengehängt. Diese Regel ist nur wirksam, wenn <flag> verschieden von 0
ist, also der abschließende Zeilenvorschub ignoriert wird.

Falls |$1| mit <flag> |== 0| aufgerufen wurde, werden Leerzeichen und
Tabulatoren am Anfang des Kommentars entfernt.
*/

int io_skipcom (IO *io, StrBuf *buf, int flag)
{
	char *prompt;
	int startline;
	int c;

	startline = 1;
	indent = 0;

	while ((c = io_getc(io)) == '/')
	{
		switch ((c = io_getc(io)))
		{
		case '*':
			prompt = io_prompt(io, PROMPT);

			if	(buf)
			{
				sb_clean(buf);
				indent = 0;
				save_cstyle(io, buf, flag);
				startline = 1;
			}
			else	skip_cstyle(io);

			io_prompt(io, prompt);

			if	(flag && io_peek(io) == '\n')
				io_getc(io);
			break;
		case '/':
			if	(buf)
			{
				if	(startline)
				{
					sb_clean(buf);
					indent = 0;
				}

				c = save_line(io, buf);
				startline = 0;
			}
			else	c = skip_line(io);

			if	(!flag || c == EOF)
				return c;

			break;
		default:
			io_ungetc(c, io);
			/* FALLTROUGH */
		case EOF:
			return '/';
		}
	}

	return c;
}

/*
$SeeAlso
\mref{io_getc(3)}.
*/

static void skip_cstyle (IO *io)
{
	int last, c;

	for (last = 0; (c = io_getc(io)) != EOF; last = c)
		if (c == '/' && last == '*')
			break;
}

static int skip_line (IO *io)
{
	int last, c;

	for (last = 0; (c = io_getc(io)) != EOF; last = c)
		if (c == '\n' && last != '\\')
			return c;

	return EOF;
}


static void save_cstyle (IO *io, StrBuf *buf, int flag)
{
	int last, c;
	int pos;

	pos = 2;

/*	Sterne am Anfang ignorieren, Test auf Leerkommentar
*/
	if	((c = io_getc(io)) == '*')
	{
		do
		{
			pos++;
			c = io_getc(io);
		}
		while (c == '*');

		if	(c == '/')	return;
	}

/*	Falls nicht am Zeilenanfang: Leerzeichen ignorieren
*/
	if	(!flag)
	{
		while (c == ' ' || c == '\t')
			c = io_getc(io);

		pos = 0;
	}

/*	Ersten Backslash und Leerzeilen am Anfang ignorieren
*/
	if	(c == '\\')
	{
		c = io_getc(io);
		pos++;
	}

	while (c == '\n')
	{
		c = io_getc(io);
		pos = 0;
		flag = 1;
	}

/*	Hauptschleife
*/
	io_ungetc(c, io);

	for (last = 0; (c = io_getucs(io)) != EOF; last = c)
	{
		if (c == '/' && last == '*')
			break;

		if	(c == '\n')
		{
			pos = 0;
			flag = 1;
		}
		else if	(flag)
		{
			if	(c == '*' && pos == 1)
			{
				if	(io_peek(io) == '/')
				{
					io_getc(io);
					break;
				}

				c = ' ';
			}

			if	(c == ' ')	pos++;
			else if	(c == '\t')	pos = 8 + 8 * (pos / 8);
			else			flag = 0;

			if	(indent && pos > indent)
				flag = 0;

			if	(flag)		continue;
			if	(!indent)	indent = pos;
			if	(buf->pos)	;
		}
		else if	(indent > pos)	indent = pos;

		sb_putucs(c, buf);
	}

/*	Sterne und letzten Backslash am Ende ignorieren
*/
	while (buf->pos && buf->data[buf->pos - 1] == '*')
		buf->pos--;

	if	(buf->pos && buf->data[buf->pos - 1] == '\\')
		buf->pos--;

	sb_nul(buf);
}

static int save_line (IO *io, StrBuf *buf)
{
	int flag;
	int pos;
	int c;
	int last;

	pos = 2;
	flag = 1;

	for (last = 0; (c = io_getucs(io)) != EOF; last = c)
	{
		if	(c == '\n')
		{
			if	(last == '\\')
			{
				io_linemark(io);
				buf->pos--;
				buf->nfree++;
				continue;
			}

			sb_nul(buf);
			return c;
		}

		if	(flag)
		{
			if	(c == '*' && pos == 1)
				c = ' ';

			if	(c == ' ')	pos++;
			else if	(c == '\t')	pos = 8 + 8 * (pos / 8);
			else			flag = 0;

			if	(indent && pos > indent)
				flag = 0;

			if	(flag)		continue;
			if	(!indent)	indent = pos;
			if	(buf->pos)	sb_putc('\n', buf);
		}
		else if	(indent > pos)	indent = pos;

		sb_putucs(c, buf);
	}

	sb_nul(buf);
	return EOF;
}
