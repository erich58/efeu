/*
Dokumentstruktur

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

#include <efeudoc.h>

/*	Referenzstruktur
*/

static Doc_t *doc_admin (Doc_t *tg, const Doc_t *src)
{
	if	(tg)
	{
		io_close(tg->out);

		if	(tg->buf)
			del_strbuf(tg->buf);

		while (tg->cmd_stack)
			rd_deref(popstack(&tg->cmd_stack, NULL));

		memfree(tg);
		return NULL;
	}

	tg = memalloc(sizeof(Doc_t));
	tg->buf = new_strbuf(0);
	tg->out = NULL;
	tg->nl = 1;
	pushstack(&tg->cmd_stack, rd_refer(GlobalDocTab));
	return tg;
}

static char *doc_ident (Doc_t *doc)
{
	strbuf_t *sb;
	io_t *io;
	char *p;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	io_printf(io, "type = %#s", doc->type ? doc->type->name : NULL);
	p = rd_ident(doc->out);
	io_printf(io, ", out = %s", p);
	memfree(p);
	io_printf(io, ", stat = %d", doc->stat);
	io_printf(io, ", indent = %d", doc->indent);
	io_close(io);
	return sb2str(sb);
}

ADMINREFTYPE(Doc_reftype, "Doc", doc_ident, doc_admin);

Doc_t *Doc (const char *type)
{
	Doc_t *doc = rd_create(&Doc_reftype);
	doc->type = GetDocType(type);
	return doc;
}
