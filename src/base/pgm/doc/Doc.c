/*	Dokumentstruktur
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
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

REFTYPE(Doc_reftype, "Doc", doc_ident, doc_admin);

Doc_t *Doc (const char *type)
{
	Doc_t *doc = rd_create(&Doc_reftype);
	doc->type = GetDocType(type);
	return doc;
}
