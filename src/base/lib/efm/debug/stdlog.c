/*
Standardfunktionen zur Meldungsausgabe

$Name log_psub, log_psubarg, log_psubvarg, log_note, log_error

$Copyright (C) 2009 Erich Frühstück
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

#include <EFEU/Debug.h>
#include <EFEU/mstring.h>
#include <EFEU/CmdPar.h>
#include <EFEU/Resource.h>
#include <EFEU/ioctrl.h>

/*
Die Funktion |$1| gibt die Meldung <fmt> für die Protokollklasse <ctrl> aus.
Die Substitutionsparameter werden in Form einer einer Argumentliste übergegen.
Diese Funktion stellt die Basis für alle Funktionen zur Ausgabe
von Fehlermeldungen dar und wird selten direkt verwendet.
*/

void log_psub (LogControl *ctrl, const char *fmt, ArgList *args)
{
	IO *io;
	char *xfmt;

	if	(!(io = LogOpen(ctrl)))
		return;

	if	(!(xfmt = GetFormat(fmt)))
	{
		switch (ctrl->level)
		{
		case LOGLEVEL_ERR:	io_puts("ERROR", io); break;
		case LOGLEVEL_WARN:	io_puts("WARNING", io); break;
		case LOGLEVEL_NOTE:	io_puts("NOTE", io); break;
		case LOGLEVEL_INFO:	io_puts("INFO", io); break;
		case LOGLEVEL_DEBUG:	io_puts("DEBUG", io); break;
		case LOGLEVEL_TRACE:	io_puts("TRACE", io); break;
		default:		io_puts("MESSAGE", io); break;
		}

		if	(fmt && *fmt)
		{
			io_putc(' ', io);
			io_puts(fmt, io);
		}

		io_putc(':', io);

		if	(args && args->dim)
		{
			int i;

			for (i = 0; i < args->dim; i++)
				io_xprintf(io, " $%d=%#s", i,
					StrPool_get((StrPool *) args,
						args->index[i]));
		}

		io_putc('\n', io);
	}
	else	CmdPar_psubout(NULL, io, xfmt, args);

	io_close(io);
}

/*
Die Funktion |$1| gibt die Meldung <fmt> für die Protokollklasse <ctrl> aus.
Die Substitutionsparameter werden in einer Argumentliste zusammengestellt.
Der Parameter 0 wird durch <id> bestimmt, <id> muß entweder |NULL| sein
oder auf einen mit |memalloc()| eingerichteten Speicher verweisen.
Alle anderen Parameter werden über <argdef> an die
Argumentliste angehängt.

Diese Funktion stellt die Basis für alle Funktionen zur Ausgabe
von Fehlermeldungen mit einer variablen Argumentliste dar und wird
selten direkt verwendet.
*/

void log_psubvarg (LogControl *ctrl, const char *fmt,
	char *id, const char *argdef, va_list list)
{
	if	(argdef || id)
	{
		ArgList *args = arg_create();
		arg_madd(args, id);
		arg_append(args, argdef, list);
		log_psub(ctrl, fmt, args);
		rd_deref(args);
	}
	else	log_psub(ctrl, fmt, NULL);
}


void log_psubarg (LogControl *ctrl, const char *fmt,
	char *id, const char *argdef, ...)
{
	va_list list;
	va_start(list, argdef);
	log_psubvarg(ctrl, fmt, id, argdef, list);
	va_end(list);
}


/*
Die Funktion |$1| gibt eine Meldung zur Fehlerklasse <cl> vom Level
|DBG_NOTE| aus. Der Parameter 0 kann nicht vorgegeben werden und
hat immer den Wert |NULL|.
*/

void log_note (LogControl *ctrl, const char *fmt, const char *argdef, ...)
{
	va_list list;
	va_start(list, argdef);
	log_psubvarg(ctrl ? ctrl : NoteLog, fmt, NULL, argdef, list);
	va_end(list);
}

/*
Die Funktion |$1| gibt eine Meldung zur Fehlerklasse <cl> vom Level
|DBG_ERR| aus und bricht anschließend die Verarbeitung mit einem Aufruf von
|exit(EXIT_FAILURE)| ab. Der Parameter 0 kann nicht vorgegeben werden und hat
immer den Wert |NULL|.
*/

void log_error (LogControl *ctrl, const char *fmt, const char *argdef, ...)
{
	va_list list;
	va_start(list, argdef);
	log_psubvarg(ctrl ? ctrl : ErrLog, fmt, NULL, argdef, list);
	va_end(list);
	exit(EXIT_FAILURE);
}

/*
$SeeAlso
\mref{Debug(3)},
\mref{arg_append(3)},
\mref{CmdPar_psubout(3)},
\mref{Debug(7)}.
*/

