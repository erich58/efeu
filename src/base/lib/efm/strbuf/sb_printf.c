/*
:*:printf for string buffer
:de:Printf für Stringbuffer

$Copyright (C) 1994 Erich Frühstück
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

#include <EFEU/strbuf.h>
#include <EFEU/io.h>

/*
:*:
The function |$1| allows formatted printing to string buffers.
See |io_printf| for a detailed description.
:de:
The Funktion |$1| erlaubt formatierte Ausgaben in den Stringbuffer <sb>.
Vergleiche dazu |io_printf| für eine detailierte Beschreibung.
*/

int sb_printf (StrBuf *sb, const char *fmt, ...)
{
	if	(sb && fmt)
	{
		va_list args;
		IO *io;
		int n;

		io = io_strbuf(sb);
		va_start(args, fmt);
		n = io_vprintf(io, fmt, args);
		va_end(args);
		io_close(io);
		return n;
	}
	else	return 0;
}

/*
:*:
The function |$1| does the same as |sb_printf| with an argument of |va_list|
instead of a variable argument list.
:de:
Die Funktion |$1| hat die selbe Funktionalität wie |sb_printf|, nur wird
ein Argument vom Typ |va_list| anstelle einer variablen Argumentliste
verwendet.
*/

int sb_vprintf (StrBuf *sb, const char *fmt, va_list list)
{
	if	(sb && fmt)
	{
		IO *io;
		int n;

		io = io_strbuf(sb);
		n = io_vprintf(io, fmt, list);
		io_close(io);
		return n;
	}
	else	return 0;
}

/*
$SeeAlso
\mref{io_printf(3)},
\mref{strbuf(3)},
\mref{strbuf(7)}.
*/
