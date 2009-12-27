/*
Interaktive Ein/Ausgabe

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

#include <EFEU/ioctrl.h>
#include <EFEU/extension.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>

#define	PROMPT_STD	"$!: "
#define	PROMPT_SUB	"> "
#define	PROMPT_MARK	"> "

#if	HAS_ISATTY
int isatty (int field);

#endif

static int is_interactive (void)
{
#if	HAS_ISATTY
	return (isatty(0) && isatty(1));
#else
	return 0;
#endif
}

/*	Hilfsvariablen für Standardeingabe
*/

static int line = 0;
static int mode_depth = 0;
static int iflag = 1;
static int mark = 1;
static char *prompt = NULL;


/*	Hilfsfunktionen
*/

static int ia_get (void *io);
static int ia_put (int c, void *io);
static int ia_ctrl (void *io, int req, va_list list);


/*	Lesen von der Standardeingabe
*/

static int ia_get (void *ptr)
{
	int c;

	if	(iflag)
	{
		char *p;

		if	(mark)		p = PROMPT_MARK;
		else if	(mode_depth)	p = PROMPT_SUB;
		else if	(prompt)	p = prompt;
		else			p = PROMPT_STD;

		line++;
		io_psubarg(iostd, p, NULL);
	}

	c = getchar();

	if	(c == EOF)
	{
		ia_put('\n', ptr);
	}
	else	iflag = (c == '\n');

	return c;
}


/*	Zeichen nach Standardausgabe schreiben
*/

static int ia_put (int c, void *ptr)
{
	return putchar(c);
}


/*	Kontrollfunktion
*/

static int ia_ctrl (void *par, int req, va_list list)
{
	char **ptr;

	switch (req)
	{
	case IO_LINEMARK:
		
		mark = 1;
		return 0;

	case IO_PROTECT:
	case IO_SUBMODE:
	
		if	(va_arg(list, int))	mode_depth++;
		else if	(mode_depth > 0)	mode_depth--;

		return mode_depth;

	case IO_IDENT:

		*va_arg(list, char **) = msprintf("<interact>:%d",
			line ? line : 1);
		return 0;

	case IO_PROMPT:

		if	((ptr = va_arg(list, char **)) != NULL)
		{
			char *save = prompt;
			prompt = *ptr;
			*ptr = save;
			return 0;
		}
		
		return EOF;

	case IO_LINE:

		return (line ? line : 1);

	case IO_ERROR:

		return 0;

	default:

		break;
	}

	return EOF;
}


IO *(*_interact_open) (const char *prompt, const char *hist) = NULL;
IO *(*_interact_filter) (IO *io) = NULL;

/*
Die Funktion |$1| öffnet eine IO-Struktur, die |stdin| als
Eingabe und |stdout| für die Ausgabe verwendet. Das Argument
<prompt> legt den Prompt für die Eingabe fest.

Falls <histname> verschieden von Null ist und eine Unterstützung
von History-Zeilen vorhanden ist, wird die entsprechende Datei
als Historyfile verwendet.

Falls Standardeingabe oder Standardausgabe nicht mit einem Terminal
verbunden sind, wird kein Prompt ausgegeben.
*/

IO *io_interact (const char *prompt, const char *hist)
{
	IO *io;

	if	(_interact_open)
	{
		io = _interact_open(prompt, hist);
	}
	else if	(is_interactive())
	{
		io = io_alloc();
		io->get = ia_get;
		io->put = ia_put;
		io->ctrl = ia_ctrl;
		prompt = mstrcpy(prompt);
	}
	else	io = io_batch();

	return _interact_filter ? _interact_filter(io) : io;
}

/*
$SeeAlso
\mref{SetupReadline(3)},
\mref{io(7)}, \mref{Readline(7)}.
*/
