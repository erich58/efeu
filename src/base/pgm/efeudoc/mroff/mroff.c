/*
Ausgabefilter für mroff

$Copyright (C) 1999 Erich Frühstück
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

#include <mroff.h>
#include <efeudoc.h>
#include <ctype.h>

#define	NAME	"mroff"	/* Name der Variablentabelle */
#define	HDRCFG	"mroff"	/* Kopfkonfiguration */
#define	SYMTAB	"mroff"	/* Sonderzeichentabelle */

IO *DocOut_mroff (IO *io)
{
	ManRoff *mr = DocDrv_alloc(NAME, sizeof(ManRoff));
	mroff_setup();
	mr->out = io;
	mr->last = '\n';
	mr->symtab = DocSym_load(SYMTAB);
	mr->put = mroff_putc;
	mr->hdr = mroff_hdr;
	mr->rem = mroff_rem;
	mr->cmd = mroff_cmd;
	mr->env = mroff_env;
	return DocDrv_io(mr);
}
