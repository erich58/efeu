/*
Eine andere IO-Struktur vor eine IO-Struktur setzen

$Copyright (C) 1999 Erich Fr�hst�ck
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
#include <EFEU/stack.h>
#include <EFEU/mstring.h>

typedef struct {
	io_t save;
	io_t *tmp;
} PUSHPAR;

static PUSHPAR *push_create (io_t *io, io_t *tmp)
{
	PUSHPAR *par = memalloc(sizeof(PUSHPAR));
	par->save = *io;
	par->tmp = tmp;
	io->stat = 0;
	io->nsave = 0;
	return par;
}

static int push_restore (PUSHPAR *par, va_list list)
{
	io_t *io = va_arg(list, io_t *);
	io_t **ptr = va_arg(list, io_t **);
	int i;

	for (i = 0; i < io->nsave; i++)
		io_ungetc(io->save_buf[i], par->tmp);

	par->save.refcount = io->refcount;
	*io = par->save;

	if	(ptr)
	{
		*ptr = par->tmp;
		return 1;
	}

	rd_deref(par->tmp);
	return 0;
}

static int push_ident (PUSHPAR *par, char **ptr)
{
	if	(ptr)
	{
		char *base = io_ident(&par->save);
		char *name = io_ident(par->tmp);
		*ptr = mstrpaste(", ", base, name);
		memfree(base);
		memfree(name);
	}

	return 0;
}


/*	IO - Funktionen
*/

static int push_get (PUSHPAR *par)
{
	return io_getc(par->tmp);
}

static int push_put (int c, PUSHPAR *par)
{
	return io_putc(c, par->tmp);
}

static int push_ctrl (PUSHPAR *par, int req, va_list list)
{
	switch (req)
	{
	case IO_RESTORE:	return push_restore(par, list);
	case IO_UNGETC:		return io_ungetc(va_arg(list, int), par->tmp);
	case IO_IDENT:		return push_ident(par, va_arg(list, char **));
	default:		return io_vctrl(par->tmp, req, list);
	}
}


/*	IO-Struktur vorsetzen
*/

void io_push (io_t *io, io_t *tmp)
{
	if	(io && tmp)
	{
		io->data = push_create(io, tmp);
		io->get = (io_get_t) push_get;
		io->put = (io_put_t) push_put;
		io->ctrl = (io_ctrl_t) push_ctrl;
	}
	else	rd_deref(tmp);
}

/*	Vorgesetzte Struktur entfernen
*/

io_t *io_pop (io_t *io)
{
	return (io_ctrl(io, IO_RESTORE, io, &io) != EOF) ? io : NULL;
}
