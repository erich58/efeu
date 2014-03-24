/*
Makrodefinition

$Copyright (C) 1994, 2002 Erich Frühstück
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

#include <EFEU/object.h>
#include <EFEU/preproc.h>
#include <ctype.h>

static void getmacarg (IO *io, PPMacro *mac, StrBuf *buf);


PPMacro *ParseMacro (IO *io)
{
	PPMacro *mac;
	StrBuf *buf;
	int c;

	mac = NewMacro();
	buf = sb_acquire();

	while ((c = io_skipcom(io, NULL, 0)) != EOF)
	{
		if	(c == '_' || isalnum(c))
			sb_putc(c, buf);
		else	break;
	}

	mac->name = sb_strcpy(buf);

	if	(c == '(')
	{
		getmacarg(io, mac, buf);
		mac->sub = macsub_subst;
	}
	else
	{
		mac->sub = macsub_repl;
		io_ungetc(c, io);
	}

	sb_setpos(buf, 0);

	do	c = io_skipcom(io, NULL, 0);
	while	(c == ' ' || c == '\t');

	while (c != EOF && c != '\n')
	{
		if	(c == '\\')
		{
			if	((c = io_getc(io)) == EOF)
			{
				c = '\\';
			}
			else	sb_putc('\\', buf);
		}

		sb_putc(c, buf);
		c = io_skipcom(io, NULL, 0);
	}

	io_ungetc(c, io);
	mac->repl = sb_cpyrelease(buf);
	return mac;
}


static void getmacarg (IO *io, PPMacro *mac, StrBuf *buf)
{
	PPMacro *x;
	IO *tmp;
	int c;
	int i;

	tmp = io_tmpbuf(0);
	mac->hasarg = 1;
	mac->dim = 0;
	PushMacroTab(NULL);
	sb_setpos(buf, 0);

	for (;;)
	{
		c = io_skipcom(io, NULL, 0);

		if	(c == ',' || c == ')' || c == EOF)
		{
			if	(sb_getpos(buf))
			{
				sb_putc(0, buf);
				x = NewMacro();
				x->name = sb_memcpy(buf);
				x->sub = macsub_repl;
				AddMacro(x);
			}
			else	x = NULL;

			io_write(tmp, &x, sizeof(PPMacro *));
			sb_setpos(buf, 0);
			mac->dim++;

			if	(c != ',')	break;
		}
		else if	(!isspace(c))
		{
			sb_putc(c, buf);
		}
	}

	if	(mac->dim != 0)
	{
		mac->tab = PopMacroTab();
		mac->arg = memalloc(mac->dim * sizeof(PPMacro *));
		io_rewind(tmp);

		for (i = 0; i < mac->dim; i++)
			io_read(tmp, mac->arg + i, sizeof(PPMacro *));

		i = mac->dim - 1;

		if	(mac->arg[i] && strcmp("...", mac->arg[i]->name) == 0)
		{
			memfree(mac->arg[i]->name);
			mac->arg[i]->name = "va_list";
			mac->vaarg = 1;
		}
	}

	io_close(tmp);
}
