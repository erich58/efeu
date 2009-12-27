/*
:*:	ouput string language filter
:de:	Ausgabe eines Strings mit Sprachfilter

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

#include <EFEU/io.h>

/*
:*:
The function |$1| writes the locale specific part of <str> to <out>.
:de:
Die Funktion |$1| filtert die zur lokalen Sprache zugehörigen Teile
aus dem String <str> und gibt in nach <out> aus.
*/

int io_langputs (const char *str, io_t *out)
{
	io_t *in = langfilter(io_cstr(str), NULL);
	int n = io_copy(in, out);
	io_close(in);
	return n;
}

/*
$SeeAlso
\mref{langfilter(3)},
\mref{io_puts(3)},
\mref{io(7)},
\mref{LangType(7)}.\br
*/
