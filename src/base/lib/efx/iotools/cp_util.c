/*
Kopierhilfsfunktionen

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

#include <EFEU/efmain.h>
#include <EFEU/efio.h>


/*	Einsetzen eines Wertes
*/

int iocpy_repl(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	return io_puts(arg, out);
}


/*	Zeichen Markieren
*/

int iocpy_mark(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int n;

	n = io_puts(arg, out);
	io_putc(c, out);
	return n + 1;
}


/*	Namen kopieren
*/

int iocpy_name(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int n;

	io_putc(c, out);

	for (n = 1; (c = io_mgetc(in, 1)) != EOF; n++)
	{
		if	(!listcmp(arg, c))
		{
			io_ungetc(c, in);
			break;
		}

		io_putc(c, out);
	}

	return n;
}


/*	Escape - Zeichen
*/

int iocpy_esc(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int x;
	int n;

	x = io_getc(in);
	n = 0;

	if	(!listcmp(arg, x))
	{
		io_ungetc(x, in);
		return EOF;
	}

	if	(flags)	io_putc(c, out);
	
	io_putc(x, out);
	return flags ? 2 : 1;
}
