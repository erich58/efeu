/*
Ausgabefilter für LaTeX

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

#include <LaTeX.h>
#include <efeudoc.h>
#include <ctype.h>

#define	NAME	"LaTeX"		/* Name der Variablentabelle */
#define	HDRCFG	"LaTeX"		/* Kopfkonfiguration */
#define	SYMTAB	"latex"		/* Sonderzeichentabelle */


static void ltx_hdr (void *drv, int mode)
{
	LaTeX *ltx = drv;

	if	(mode)
	{
		DocDrv_eval(ltx, HDRCFG);
	}
	else	io_puts("\n\\end{document}\n", ltx->out);
}


IO *DocOut_latex (IO *io)
{
	LaTeX *ltx = DocDrv_alloc(NAME, sizeof(LaTeX));
	ltx->out = io;
	ltx->last = '\n';
	ltx->symtab = DocSym_load(SYMTAB);
	ltx->put = LaTeX_putc;
	ltx->hdr = ltx_hdr;
	ltx->rem = LaTeX_rem;
	ltx->cmd = LaTeX_cmd;
	ltx->env = LaTeX_env;
	return DocDrv_io(ltx);
}
