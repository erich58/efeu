/*
:*:Input filter with parameter substitution
:de:Eingabefilter mit Parametersubstitution

$Copyright (C) 2002 Erich Frühstück
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

#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>

#define	PSUBKEY	'$'

typedef struct {
	IO *io;		/* Eingabestruktur */
	ArgList *args;	/* Argumentliste */
	StrBuf buf;	/* Zwischenbuffer */
	char *ptr;	/* Zeichenpointer */
	int mode;	/* Verarbeitungsmodus */
	int save;	/* gesichertes Zeichen */
} PSUB;

static int psub_sget (PSUB *par)
{
	int c = *(par->ptr++);

	if	(*par->ptr == 0)
		par->ptr = NULL;

	if	(par->mode && (c == '"' || c == '\\'))
	{
		par->save = c;
		return '\\';
	}

	return c;
}

static int psub_get (IO *io)
{
	PSUB *par = io->data;
	int c;

	if	(par->save)
	{
		c = par->save;
		par->save = 0;
		return c;
	}

	if	(par->ptr)
		return psub_sget(par);

	while ((c = io_getc(par->io)) == PSUBKEY)
	{
		sb_setpos(&par->buf, 0);
		par->ptr = psubexpandarg(&par->buf, par->io, par->args);

		if	(*par->ptr)
			return psub_sget(par);

		par->ptr = NULL;
	}

	if	(c == '"')
		par->mode = !par->mode;

	return c;
}

static int psub_ctrl (IO *io, int req, va_list list)
{
	PSUB *par = io->data;
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		rd_deref(par->args);
		sb_free(&par->buf);
		stat = io_close(par->io);
		memfree(par);
		return stat;

	case IO_REWIND:

		if	(io_rewind(par->io) == EOF)
			return EOF;

		par->ptr = NULL;
		par->mode = 0;
		return 0;
		
	default:

		return io_vctrl(par->io, req, list);
	}
}

IO *psubfilter (IO *io, ArgList *args)
{
	if	(io)
	{
		PSUB *par = memalloc(sizeof(PSUB));
		par->io = io;
		par->args = args;
		sb_init(&par->buf, 0);
		par->ptr = NULL;
		par->mode = 0;

		io = io_alloc();
		io->get = psub_get;
		io->ctrl = psub_ctrl;
		io->data = par;
	}

	return io;
}
