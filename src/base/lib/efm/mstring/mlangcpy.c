/*
:*:	copy string with language filter
:de:	String mit Sprachfilter umkopieren

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/mstring.h>
#include <EFEU/io.h>

/*
:*:
The Funktion |$1| copies the language <lang> specific parts of the
string <str>.
:de:
Die Funktion |$1| kopiert die zur Sprache <lang> definierten Teile
des Strings <str>.
*/

char *mlangcpy (const char *str, const char *lang)
{
	if	(str != NULL)
	{
		IO *io;
		StrBuf *buf;
		int c;

		io = langfilter(io_cstr(str), lang);
		buf = new_strbuf(0);

		while ((c = io_getc(io)) != EOF)
			sb_putc(c, buf);

		io_close(io);
		return sb2str(buf);
	}
	else	return NULL;
}

