/*
Datensicht Parameter

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

#include <EFEU/nkt.h>
#include <EFEU/Resource.h>
#include <EFEU/Info.h>
#include <EFEU/EfiView.h>

EfiPar EfiPar_view = { "EfiView", "Data view definition" };

void EfiView_info (IO *out, const void *data)
{
	const EfiViewPar *fdef;
	char *fmt;
	IO *desc;
	int c;

	fdef = data;
	fmt = GetFormat(fdef->epc_label);
	desc = langfilter(io_cstr(fmt), NULL);

	io_puts(fdef->epc_name, out);
	io_puts(fdef->syntax, out);
	io_puts("\n\t", out);

	while  ((c = io_getc(desc)) != EOF)
	{
		if	(c == '$')
		{
			switch (c = io_getc(desc))
			{
			case '1':	io_puts(fdef->epc_name, out); break;
			default:	io_putc(c, out); break;
			}
		}
		else if	(c == '\n')
		{
			io_puts("\n\t", out);
		}
		else	io_putc(c, out);
	}

	io_close(desc);
	io_putc('\n', out);
}
