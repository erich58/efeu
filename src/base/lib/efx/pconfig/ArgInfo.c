/*
Informationen über Aufrufparameter abfragen

$Copyright (C) 1998 Erich Frühstück
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

#include <EFEU/pconfig.h>

static void printarg(io_t *io, pardef_t *def)
{
	if	(!def->name)	return;
	if	(!def->desc)	return;

	io_puts(def->name, io);
	io_puts("\t", io);
	io_psub(io, def->desc);
	io_putc('\n', io);
}

void ArgInfo (io_t *io, InfoNode_t *info)
{
	vecbuf_t *vb;
	pardef_t *p;
	size_t n;

	if	((vb = info->par) == NULL) return;

	for (n = vb->used, p = vb->data; n > 0; n--, p++)
		printarg(io, p);
}
