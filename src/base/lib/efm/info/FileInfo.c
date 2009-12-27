/*
Infotext aus Datei laden

$Copyright (C) 1998 Erich Frühstück
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

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <ctype.h>

#define	BUFSIZE	4096

static char buf[BUFSIZE];


static void FileLabel_print (io_t *io, FileLabel_t *par)
{
	FILE *file;
	int n;

	if	(par->name == NULL)	return;

	file = fopen(par->name, "r");

	if	(file == NULL)
	{
		io_printf(io, "<%s: %d", par->name, par->start + 1);

		if	(par->end > par->start + 2)
			io_printf(io, " - %d", par->end - 1);

		io_puts(">\n", io);
		return;
	}

	for (n = 1; fgets(buf, BUFSIZE, file); n++)
	{
		if	(n <= par->start)	;
		else if	(n >= par->end)		break;
		else				io_psub(io, buf);
	}

	fclose(file);
}

InfoType_t InfoType_FileLabel = { NULL, (InfoPrint_t) FileLabel_print };
