/*
Deklarationsstruktur

$Copyright (C) 2000 Erich Frühstück
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

#include "src2doc.h"
#include <EFEU/parsub.h>

int Decl_test (Decl_t *decl, const char *name)
{
	int i;

	for (i = decl->start; i < decl->end; i++, name++) 
		if (*name != decl->def[i]) return 0;

	return (*name == 0);
}	

void Decl_print (Decl_t *decl, io_t *io)
{
	io_psub(io, "\\index[$1]\n");

	switch (decl->type)
	{
	case DECL_FUNC:
		io_puts("\\hang\n", io);
		copy_protect(decl->def, io);
		io_putc(' ', io);
		copy_protect(decl->arg, io);
		io_puts(";\n\\end\n\n", io);
		break;
	case DECL_TYPE:	
	case DECL_VAR:
		io_puts("\\hang\n", io);
		copy_protect(decl->def, io);
		io_puts(";\n\\end\n\n", io);
		break;
	case DECL_STRUCT:	
		io_printf(io, "---- verbatim\n\n%s;\n----\n\n", decl->def);
		break;
	case DECL_LABEL:
	case DECL_CALL:
	default:
		copy_protect(decl->def, io);
		io_putc(' ', io);
		copy_protect(decl->arg, io);
		io_puts("\n\n", io);
		break;
	}
}
