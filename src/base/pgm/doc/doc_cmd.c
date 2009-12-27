/*	Dokumentbefehle ausführen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "efeudoc.h"
#include <ctype.h>
#include <EFEU/cmdeval.h>
#include <EFEU/preproc.h>


void Doc_eval (Doc_t *doc, io_t *in, const char *expr)
{
	strbuf_t *buf;
	io_t *save_cin;
	io_t *io, *out;

	if	(expr == NULL)	return;

	buf = new_strbuf(0);

	save_cin = CmdEval_cin;
	CmdEval_cin = rd_refer(in);

	Doc_pushvar(doc);
	io = io_cmdpreproc(io_cstr(expr));
	out = io_strbuf(buf);
	CmdEvalFunc(io, out, 0);
	io_close(io);
	io_close(out);
	Doc_popvar(doc);

	rd_deref(CmdEval_cin);
	CmdEval_cin = save_cin;

	if	(buf->pos)
	{
		io_push(in, io_mstr(sb2str(buf)));
	}
	else	del_strbuf(buf);
}

void Doc_cmd (Doc_t *doc, io_t *in)
{
	char *p;
	DocMac_t *mac;
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
		io_message(in, MSG_DOC, 12, 1, p);
		io_puts(p, doc->out);
		Doc_char(doc, ';');
	}
	else	Doc_eval(doc, in, mac->fmt);
}
