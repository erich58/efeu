/*
Ausgabefilter f�r HTML-Unixdoc

$Copyright (C) 1999 Erich Fr�hst�ck
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

#include <HTML.h>
#include <efeudoc.h>
#include <ctype.h>

#define	NAME	"html"	/* Name der Variablentabelle */
#define	HDRCFG	"html"	/* Kopfkonfiguration */
#define	SYMTAB	"html"	/* Sonderzeichentabelle */


static void html_hdr (HTML_t *html, int mode)
{
	HTML_newline(html, 0);

	if	(!mode)	return;

	io_ctrl(html->out, HTML_TITLE,
		Obj2str(GetVar(LocalVar, "title", NULL)));
	DocDrv_eval(html, HDRCFG);
}


io_t *DocOut_html (io_t *io)
{
	HTML_t *html = DocDrv_alloc(NAME, sizeof(HTML_t));
	html->out = io;
	html->class = 0;
	html->last = '\n';
	html->symtab = DocSym(SYMTAB);
	html->sym = (DocDrvSym_t) HTML_sym;
	html->put = (DocDrvPut_t) HTML_putc;
	html->hdr = (DocDrvHdr_t) html_hdr;
	html->rem = (DocDrvRem_t) HTML_rem;
	html->cmd = (DocDrvCmd_t) HTML_cmd;
	html->env = (DocDrvEnv_t) HTML_env;
	html->buf = new_strbuf(0);
	DocDrv_var(html, &Type_int, "class", &html->class);
	return DocDrv_io(html);
}
