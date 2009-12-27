/*
Dokumentbefehle ausführen

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

#include "efeudoc.h"
#include <ctype.h>
#include <EFEU/cmdeval.h>
#include <EFEU/preproc.h>

IO *Doc_preproc (IO *io)
{
	return io_ptrpreproc(io, &IncPath, &DocPath);
}

void Doc_eval (Doc *doc, IO *in, const char *expr)
{
	StrBuf buf;
	IO *save_cin;
	IO *io, *out;

	if	(expr == NULL)	return;

	sb_init(&buf, 0);

	save_cin = CmdEval_cin;
	CmdEval_cin = rd_refer(in);

	Doc_pushvar(doc);
	io = Doc_preproc(io_cstr(expr));
	out = io_strbuf(&buf);
	CmdEvalFunc(io, out, 0);
	io_close(io);
	io_close(out);
	Doc_popvar(doc);

	rd_deref(CmdEval_cin);
	CmdEval_cin = save_cin;

	if	(buf.pos)
		io_push(in, io_mstr(sb_strcpy(&buf)));

	sb_free(&buf);
}

void Doc_cmd (Doc *doc, IO *in)
{
	char *p;
	DocMac *mac;
	int c;

	c = io_getc(in);

/*	Zeilenfortsetzung
*/
	if	(c == '\n')
	{
		io_linemark(in);
		return;
	}

/*	Sonderzeichen
*/
	if	(!(isalpha(c) || c == '_'))
	{
		Doc_char(doc, c);
		return;
	}

/*	Befehle
*/
	io_ungetc(c, in);
	p = DocParseName(in, '\\');

	if	(io_peek(in) == '=')
	{
		io_getc(in);
		p = mstrcpy(p);	/* Temporärer Buffer !!! */
		DocTab_setmac(doc->cmd_stack->data, p,
			Doc_lastcomment(doc), DocParseExpr(in));
		return;
	}

	mac = Doc_getmac(doc, p);

	if	(mac == NULL)
	{
		io_note(in, "[Doc:12]", "s", p);
		io_puts(p, doc->out);
		Doc_char(doc, ';');
	}
	else	Doc_eval(doc, in, mac->fmt);
}
