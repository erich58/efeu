/*
Versionsprüfung für EFI-Datentyp

$Copyright (C) 2006 Erich Frühstück
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
#include <EFEU/parsedef.h>
#include <EFEU/EfiPar.h>

void EfiType_version (EfiType *type, IO *io)
{
	int c;
	char *p;
	EfiControl *ctrl;

	if	(!type)	return;

	c = io_eat(io, " \t");

	if	(c != '<')	return;

	io_getc(io);
	p = io_xgets(io, "\\>");
	io_getc(io);

	ctrl = GetEfiPar(type, &EfiPar_control, "version");

	if	(!ctrl)
	{
		io_note(io, "[efmain:vskip]", "sm", type->name, p);
	}
	else if	(mstrcmp(ctrl->data, p) != 0)
	{
		io_error(io, "[efmain:vdiff]", "sms",
			type->name, p, ctrl->data);
	}
	else	memfree(p);
}
