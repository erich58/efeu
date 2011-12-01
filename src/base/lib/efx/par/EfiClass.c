/*	Efi-Zähldefinitionen

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
#include <EFEU/EfiClass.h>

#define	INFO_NAME	"class"

#define	INFO_HEAD	\
	":*:object classifikation" \
	":de:Objektklassifikation"

EfiPar EfiPar_class = { "class", "object classification" };

void EfiClass_info (IO *out, const void *data)
{
	const EfiClass *fdef;
	char *fmt;
	IO *desc;
	int c;

	fdef = data;
	fmt = GetFormat(fdef->epc_label);
	desc = langfilter(io_cstr(fmt), NULL);

	io_puts(fdef->epc_name, out);

	if	(fdef->syntax)
	{
		if	(!strchr("[=]", fdef->syntax[0]))
			io_puts(" ", out);

		io_puts(fdef->syntax, out);
	}

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

extern EfiClass EfiClass_grp;

void SetupEfiClass (void)
{
	static int init_done = 0;

	if	(init_done)	return;

	init_done = 1;
	AddEfiPar(NULL, &EfiClass_flag);
	AddEfiPar(NULL, &EfiClass_generic);
	AddEfiPar(NULL, &EfiClass_test);
	AddEfiPar(NULL, &EfiClass_switch);
	EfiClassEnum();
}
