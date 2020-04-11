/*
Ausgabe von UCS-Zeichen

$Copyright (C) 2007 Erich Frühstück
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
#include <EFEU/locale.h>
#include <EFEU/mstring.h>

int io_putucs_ascii (int32_t c, IO *io)
{
	return io_nputc(c > 0xff ? '?' : c, io, 1);
}

int io_putucs_latin1 (int32_t c, IO *io)
{
	return io_nputc(c > 0xff ? '?' : c, io, 1);
}

int io_putucs_latin9 (int32_t c, IO *io)
{
	return io_nputc(ucs_to_latin9(c), io, 1);
}

int io_putucs_xml (int32_t c, IO *io)
{
	switch (c)
	{
	case '<':	return io_puts("&lt;", io);
	case '>':	return io_puts("&gt;", io);
	case '&':	return io_puts("&amp;", io);
	case '\'':	return io_puts("&apos;", io);
	case '"':	return io_puts("&quot;", io);
	default:
		break;
	}

	if	(c >= 0x7f)
		return io_xprintf(io, "&#%d;", c);

	return io_nputc(c, io, 1);
}

int io_putucs_utf8 (int32_t c, IO *io)
{
	int n = 0;

	if	(c < 0)
	{
		return 0;
	}
	else if	(c <= 0x0000007F)
	{
		n += io_nputc(c, io, 1);
	}
	else if	(c <= 0x000007FF)
	{
		n += io_nputc(0xc0 | ((c >> 6) & 0x1f), io, 1);
		n += io_nputc(0x80 | (c & 0x3f), io, 1);
	}
	else if	(c <= 0x0000FFFF)
	{
		n += io_nputc(0xe0 | ((c >> 12) & 0x0f), io, 1);
		n += io_nputc(0x80 | ((c >> 6) & 0x3f), io, 1);
		n += io_nputc(0x80 | (c & 0x3f), io, 1);
	}
	else if	(c <= 0x001FFFFF)
	{
		n += io_nputc(0xf0 | ((c >> 18) & 0x07), io, 1);
		n += io_nputc(0x80 | ((c >> 12) & 0x3f), io, 1);
		n += io_nputc(0x80 | ((c >> 6) & 0x3f), io, 1);
		n += io_nputc(0x80 | (c & 0x3f), io, 1);
	}
	else if	(c <= 0x03FFFFFF)
	{
		n += io_nputc(0xf8 | ((c >> 24) & 0x03), io, 1);
		n += io_nputc(0x80 | ((c >> 18) & 0x3f), io, 1);
		n += io_nputc(0x80 | ((c >> 12) & 0x3f), io, 1);
		n += io_nputc(0x80 | ((c >> 6) & 0x3f), io, 1);
		n += io_nputc(0x80 | (c & 0x3f), io, 1);
	}
	else
	{
		n += io_nputc(0xfc | ((c >> 30) & 0x01), io, 1);
		n += io_nputc(0x80 | ((c >> 24) & 0x3f), io, 1);
		n += io_nputc(0x80 | ((c >> 18) & 0x3f), io, 1);
		n += io_nputc(0x80 | ((c >> 12) & 0x3f), io, 1);
		n += io_nputc(0x80 | ((c >> 6) & 0x3f), io, 1);
		n += io_nputc(0x80 | (c & 0x3f), io, 1);
	}

	return n;
}

static struct {
	const char *name;
	int (*put) (int32_t c, IO *io);
} ctab [] = {
	{ "C", io_putucs_ascii },
	{ "ascii", io_putucs_ascii },
	{ "ASCII", io_putucs_ascii },
	{ "ANSI_X3.110-1983", io_putucs_ascii },
	{ "ANSI_X3.4-1968", io_putucs_ascii },
	{ "latin1", io_putucs_latin1 },
	{ "latin9", io_putucs_latin9 },
	{ "ISO-8859-1", io_putucs_latin1 },
	{ "ISO-8859-15", io_putucs_latin9 },
	{ "utf8", io_putucs_utf8 },
	{ "UTF-8", io_putucs_utf8 },
	{ "xml", io_putucs_xml },
	{ "XML", io_putucs_xml },
};

void io_ucswmode (IO *io, const char *def)
{
	int i;

	if	(def == NULL || *def == 0)
	{
#if	HAS_NL_LANGINFO
		def = nl_langinfo(CODESET);
#else
		def = "C";
#endif
	}

	for (i = 0; i < tabsize(ctab); i++)
	{
		if	(strcmp(def, ctab[i].name) == 0)
		{
			io->putucs = ctab[i].put;
			return;
		}
	}

	io->putucs = io_putucs_xml;
}

int io_putucs (int32_t c, IO *io)
{
	if	(!io)	return 0;

	if	(!io->putucs)
		io_ucswmode(io, NULL);

	return io->putucs(c, io);
}
