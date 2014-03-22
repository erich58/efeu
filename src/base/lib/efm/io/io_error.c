/*
Fehlermeldung mit IO-Struktur

$Name io_note, io_error

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
#include <EFEU/ioctrl.h>
#include <EFEU/procenv.h>
#include <EFEU/Debug.h>
#include <EFEU/CmdPar.h>

static LogControl err_log = LOG_CONTROL("IO", LOGLEVEL_ERR);
static LogControl note_log = LOG_CONTROL("IO", LOGLEVEL_NOTE);

/*
Die Funktion |$1| gibt eine Meldung zur Fehlerklasse |IO| vom Level
|DBG_NOTE| aus. Der Parameter 0 wird mit |io_ident()|
auf die Kennung der IO-Struktur <io> gesetzt.
*/

void io_note (IO *io, const char *fmt, const char *def, ...)
{
	va_list list;

	va_start(list, def);
	log_psubvarg(&note_log, fmt, rd_ident(io), def, list);
	va_end(list);
}

/*
Die Funktion |$1| gibt eine Meldung zur Fehlerklasse |IO| vom Level
|DBG_NOTE| aus. Der Parameter 0 enthält wird mit |io_ident()|
auf die Kennung der IO-Struktur <io> gesetzt. Nach der Ausgabe der Meldung
wird |io_ctrl| mit dem Parameter |IO_ERROR| aufgerufen. Falls die
IO-Struktur keine eigene Fehlerbehandlung kennt, wird das Programm mit
|exit(EXIT_FAILURE)| abgebrochen.
*/

void io_error (IO *io, const char *fmt, const char *def, ...)
{
	va_list list;

	va_start(list, def);
	log_psubvarg(&err_log, fmt, rd_ident(io), def, list);
	va_end(list);

	if	(io_ctrl(io, IO_ERROR) == EOF)
		exit(EXIT_FAILURE);
}

/*
$SeeAlso
\mref{Debug(3)},
\mref{stdbg(3)},
\mref{ioctrl(7)}.
*/

