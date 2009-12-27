/*	Befehle und Makros exandieren
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "efeudoc.h"
#include <ctype.h>

char *Doc_expand (Doc_t *doc, io_t *in, int flag)
{
	strbuf_t *buf;
	Doc_t save;
	
	buf = new_strbuf(0);
	save = *doc;
	doc->out = io_strbuf(buf);

	if	(doc->type && doc->type->filter)
		doc->out = doc->type->filter(doc->out);
	
	doc->stat = 1;
	doc->indent = 0;
	doc->nl = 1;
	memset(&doc->env, 0, sizeof(DocEnv_t));
	doc->env_stack = NULL;

	if	(flag)
	{
		doc->env.hmode = 2;
		io_ctrl(doc->out, DOC_BEG, DOC_MODE_COPY);
		Doc_copy(doc, in);
		io_ctrl(doc->out, DOC_END, DOC_MODE_COPY);
	}
	else
	{
		doc->env.hmode = 0;
		Doc_copy(doc, in);
	}

	io_close(in);

	while (doc->env_stack)
		Doc_endenv(doc);

	Doc_par(doc);
	io_close(doc->out);

	doc->out = save.out;
	doc->stat = save.stat;
	doc->indent = save.indent;
	doc->nl = save.nl;
	doc->env = save.env;
	doc->env_stack = save.env_stack;
	return sb2str(buf);
}

char *Doc_xexpand (Doc_t *doc, io_t *in)
{
	DocType_t *save;
	char *p;

	save = doc->type;
	doc->type = NULL;
	p = Doc_expand(doc, in, 1);
	doc->type = save;
	return p;
}
