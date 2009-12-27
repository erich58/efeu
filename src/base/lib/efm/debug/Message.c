/*
Meldungen ausgeben

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

#include <EFEU/Debug.h>
#include <EFEU/mstring.h>
#include <EFEU/CmdPar.h>

/*
Die Funktion |$1| gibt eine Fehlermeldung <fmt> vom Level <mode> für
die Klasse <class> aus. Die Parameter werden über einen Pointer vom Type
|ArgList_t| übergeben.
*/

void Message (const char *class, int level, const char *fmt, ArgList_t *args)
{
	io_t *io = LogOut(class, level);

	if	(io)
		CmdPar_psubout(NULL, io, fmt, args);

	rd_deref(args);
}

/*
Die Funktion |$1| gibt die Fehlermeldung <fmt> vom Level <mode> für
die Klasse <class> aus. Die Parameter werden als konstante Strings
über eine variable Argumentliste übergeben, wobei <narg> die Zahl der
Argumente angibt.
*/

void VMessage (const char *class, int level, const char *fmt, int narg, ...)
{
	io_t *io = LogOut(class, level);

	if	(io)
	{
		ArgList_t args;
		va_list list;
		int i;

		args.dim = narg;
		args.data = memalloc(narg * sizeof(char **));
		va_start(list, narg);

		for (i = 0; i < narg; i++)
			args.data[i] = va_arg(list, char *);

		va_end(list);
		CmdPar_psubout(NULL, io, fmt, &args);
		memfree(args.data);
	}
}

/*
$SeeAlso
\mref{Debug(3)},
\mref{Debug(7)}.
*/

