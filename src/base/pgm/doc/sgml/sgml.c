/*
Ausgabefilter für SGML-Unixdoc

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

#include <SGML.h>
#include <efeudoc.h>
#include <ctype.h>

#define	NAME	"sgml"	/* Name der Variablentabelle */
#define	HDRCFG	"sgml"	/* Kopfkonfiguration */
#define	SYMTAB	"sgml"	/* Sonderzeichentabelle */


static void sgml_hdr (SGML_t *sgml, int mode)
{
	SGML_newline(sgml, 0);

	if	(mode)
	{
		DocDrv_eval(sgml, HDRCFG);
	}
	else	io_puts(sgml->class ? "</report>\n" : "</article>\n", sgml->out);
}


io_t *DocOut_sgml (io_t *io)
{
	SGML_t *sgml = DocDrv_alloc(NAME, sizeof(SGML_t));
	sgml->out = io;
	sgml->class = 0;
	sgml->last = '\n';
	sgml->symtab = DocSym(SYMTAB);
	sgml->sym = (DocDrvSym_t) SGML_sym;
	sgml->put = (DocDrvPut_t) SGML_putc;
	sgml->hdr = (DocDrvHdr_t) sgml_hdr;
	sgml->rem = (DocDrvRem_t) SGML_rem;
	sgml->cmd = (DocDrvCmd_t) SGML_cmd;
	sgml->env = (DocDrvEnv_t) SGML_env;
	DocDrv_var(sgml, &Type_int, "class", &sgml->class);
	return DocDrv_io(sgml);
}
