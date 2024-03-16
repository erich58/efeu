/*
Dokumentstruktur einrichten

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

static void doc_clean (void *data)
{
	Doc *tg = data;
	io_close(tg->out);
	sb_free(&tg->buf);

	while (tg->cmd_stack)
		rd_deref(popstack(&tg->cmd_stack, NULL));

	memfree(tg);
}

static char *doc_ident (const void *data)
{
	const Doc *doc;
	StrBuf *sb;
	IO *io;
	char *p;

	doc = data;
	sb = sb_acquire();
	io = io_strbuf(sb);
	io_xprintf(io, "type = %#s", doc->type ? doc->type->name : NULL);
	p = rd_ident(doc->out);
	io_xprintf(io, ", out = %s", p);
	memfree(p);
	io_xprintf(io, ", stat = %d", doc->stat);
	io_xprintf(io, ", indent = %d", doc->indent);
	io_close(io);
	return sb_cpyrelease(sb);
}

static RefType doc_reftype = REFTYPE_INIT("Doc", doc_ident, doc_clean);

Doc *Doc_create (const char *type)
{
	Doc *doc = memalloc(sizeof(Doc));
	doc->type = GetDocType(type);
	sb_init(&doc->buf, 0);
	doc->out = NULL;
	doc->nl = 1;
	pushstack(&doc->cmd_stack, rd_refer(GlobalDocTab));
	return rd_init(&doc_reftype, doc);
}
