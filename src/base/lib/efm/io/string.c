/*
Ein/Ausgabe auf String, Konvertierungsbuffer

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


#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>

/*	Stringstruktur
*/

typedef struct {
	void (*clean) (void *ptr);
	unsigned char *str;
	unsigned pos;
} STRIO;

static STRIO *new_strio (char *str, void (*clean) (void *ptr))
{
	STRIO *s = memalloc(sizeof(STRIO));
	s->clean = clean;
	s->str = (unsigned char *) str;
	s->pos = 0;
	return s;
}

static void del_strio (STRIO *s)
{
	if	(s->clean)	s->clean(s->str);

	memfree(s);
}


static char *strio_ident (STRIO *s)
{
	if	(strlen((char *) s->str) > 20)
	{
		char *a = msprintf("%.20s...", (char *) s->str);
		char *b = msprintf("%#s:%d", a, s->pos);
		memfree(a);
		return b;
	}

	return msprintf("%#s:%d", (char *) s->str, s->pos);
}

	
/*	IO - Struktur
*/

static int str_get (IO *io)
{
	STRIO *s = io->data;
	return s->str[s->pos] ? (int) s->str[s->pos++] : EOF;
}

static int str_ctrl (IO *io, int req, va_list list)
{
	STRIO *s = io->data;

	switch (req)
	{
	case IO_CLOSE:
		del_strio(s);
		return 0;
	case IO_REWIND:
		s->pos = 0;
		return 0;
	case IO_GETPOS:
		*va_arg(list, unsigned *) = s->pos;
		return 0;
	case IO_SETPOS:
		{
			int n = strlen((char *) s->str);
			int pos = *va_arg(list, unsigned *);

			if	(pos < n)
			{
				s->pos = pos;
				return 0;
			}
			else	return EOF;
		}
	case IO_IDENT:
		*va_arg(list, char **) = strio_ident(s);
		return 0;
	default:
		return EOF;
	}
}

IO *io_string (char *str, void (*clean) (void *ptr))
{
	if	(str)
	{
		IO *io = io_alloc();
		io->get = str_get;
		io->ctrl = str_ctrl;
		io->data = new_strio(str, clean);
		return io;
	}

	return NULL;
}
