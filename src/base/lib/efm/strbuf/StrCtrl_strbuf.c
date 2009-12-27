/*
:*:strings with string buffer
:de:Zeichenketten mit Stringbuffer

$Header	<EFEU/$1>

$Copyright (C) 2007 Erich Frühstück
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

#include <EFEU/StrData.h>
#include <EFEU/strbuf.h>

static void sbuf_init (StrData *sd, const char *str)
{
	StrBuf *sb = sb_create(0);
	sd->ptr = sb;
	sb_puts(str, sb);
	sb_putc(0, sb);
	sd->key = 0;
}

static char *sbuf_get (const StrData *sd)
{
	const StrBuf *sb = sd->ptr;
	return (sd->key < sb->pos) ? (char *) (sb->data + sd->key) : NULL;
}

static void sbuf_set (StrData *sd, const char *str)
{
	StrBuf *sb = sd->ptr;
	sd->key = sb_getpos(sb);
	sb_puts(str, sb);
	sb_putc(0, sb);
}

static void sbuf_clean (StrData *sd, int destroy)
{
	if	(destroy)
	{
		rd_deref(sd->ptr);
		sd->ctrl = NULL;
		sd->ptr = NULL;
	}
	else	sb_setpos(sd->ptr, 0);

	sd->key = 0;
}

StrCtrl StrCtrl_strbuf = { "strbuf", "string buffer",
	sbuf_init, sbuf_clean, sbuf_get, sbuf_set, NULL,
};

void StrData_strbuf (StrData *data)
{
	if	(data->ctrl != &StrCtrl_strbuf)
	{
		StrData_clean(data, 1);
		data->ctrl = &StrCtrl_strbuf;
		data->ptr = sb_create(0);
	}
	else	sb_setpos(data->ptr, 0);

	data->key = 0;
}
