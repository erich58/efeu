/*
Teileingabedatei

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <efeudoc.h>


/*	Eingabestruktur
*/

typedef struct {
	IO *io;		/* Eingabestruktur */
	char *key;	/* Abschlußkennung */
	size_t save;	/* Zahl der gepufferten Zeichen */
	StrBuf buf;	/* Zeichenbuffer */
} SUBIO;


/*	Zeichen lesen
*/

static int subio_get (IO *io)
{
	SUBIO *subio = io->data;
	int c;

	switch (subio->save)
	{
	case 0:
		break;
	case 1:
		subio->save--;
		return '\n';
	default:
		subio->save--;
		return sb_getc(&subio->buf);
	}

	sb_begin(&subio->buf);

	while ((c = io_getc(subio->io)) != EOF)
	{
		if	(c == '\r')	continue;
		if	(c == '\n')	break;

		if	(c == '\t')
		{
			do	sb_putc(' ', &subio->buf);
			while	(sb_getpos(&subio->buf) % 8 != 0);
		}
		else	sb_putc(c, &subio->buf);
	}

	if	(sb_getpos(&subio->buf) == 0)
		return c;

	sb_putc(0, &subio->buf);
	subio->save = sb_getpos(&subio->buf);

	if	(subio->key)
	{
		if	(patcmp(subio->key, sb_nul(&subio->buf), NULL))
			return EOF;
	}
	else if	(subio->save == 1)
	{
		return EOF;
	}

	sb_begin(&subio->buf);
	subio->save--;
	return sb_getc(&subio->buf);
}


/*	Kontrollfunktion
*/

static int subio_ctrl (IO *io, int req, va_list list)
{
	SUBIO *subio = io->data;
	int stat;

	switch (req)
	{
	case IO_CLOSE:
		stat = io_close(subio->io);
		sb_free(&subio->buf);
		memfree(subio->key);
		memfree(subio);
		return stat;
	case IO_REWIND:
	case IO_PEEK:
		return EOF;
	default:
		return io_vctrl(subio->io, req, list);
	}
}


/*	Eingabestruktur
*/

IO *Doc_subin (IO *io, const char *key)
{
	if	(io)
	{
		SUBIO *subio = memalloc(sizeof(SUBIO));
		subio->io = io;
		sb_init(&subio->buf, 0);
		subio->key = mstrcpy(key);
		subio->save = 0;
		io = io_alloc();
		io->get = subio_get;
		io->ctrl = subio_ctrl;
		io->data = subio;
	}

	return io;
}
