/*
Lexikalische Sortierordnung

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

#include <EFEU/patcmp.h>
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>

static char *lextab[] = {
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	" ",	"!",	"\"",	"#",	"$",	"%",	"&",	"'",
	"(",	")",	"*",	"+",	",",	"-",	".",	"/",
	"0",	"1",	"2",	"3",	"4",	"5",	"6",	"7",
	"8",	"9",	":",	";",	"<",	"=",	">",	"?",
	"@",	"a",	"b",	"c",	"d",	"e",	"f",	"g",
	"h",	"i",	"j",	"k",	"l",	"m",	"n",	"o",
	"p",	"q",	"r",	"s",	"t",	"u",	"v",	"w",
	"x",	"y",	"z",	"[",	"\\",	"]",	"^",	"_",
	"`",	"a",	"b",	"c",	"d",	"e",	"f",	"g",
	"h",	"i",	"j",	"k",	"l",	"m",	"n",	"o",
	"p",	"q",	"r",	"s",	"t",	"u",	"v",	"w",
	"x",	"y",	"z",	"{",	"|",	"}",	"~",	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	"a",	"a",	"a",	"a",	"ae",	"a",	"ae",	"c",
	"e",	"e",	"e",	"e",	"i",	"i",	"i",	"i",
	NULL,	"n",	"o",	"o",	"o",	"o",	"oe",	"x",
	NULL,	"u",	"u",	"u",	"ue",	"y",	NULL,	"ss",
	"a",	"a",	"a",	"a",	"ae",	"a",	"ae",	"c",
	"e",	"e",	"e",	"e",	"i",	"i",	"i",	"i",
	NULL,	"n",	"o",	"o",	"o",	"o",	"oe",	NULL,
	NULL,	"u",	"u",	"u",	"ue",	"y",	NULL,	"y",
};

static STRBUF(lexbuf1, 64);
static STRBUF(lexbuf2, 64);

char *lexsortkey (const char *base, strbuf_t *buf)
{
	if	(!base)	return NULL;
	if	(!buf)	buf = &lexbuf1;

	sb_clear(buf);

	for (; *base != 0; base++)
		sb_puts(lextab[(uchar_t) *base], buf);

	sb_putc(0, buf);
	return buf->data;
}

int lexcmp (const char *a, const char *b)
{
	if	(a == b)	return 0;
	if	(a == NULL)	return -1;
	if	(b == NULL)	return 1;

	return mstrcmp(lexsortkey(a, &lexbuf1), lexsortkey(b, &lexbuf2));
}
