/*	Dokumentfunktion: Parametersubstitution
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>


void DocSubEval_psub (Document_t *doc, int c)
{
	if	(io_peek(doc->in) != c)
	{
		int save_expand;
		io_t *save_in, *buf;

		buf = io_tmpbuf(0);
		save_expand = io_ctrl(doc->in, IOPP_EXPAND, 1);
		PushVarTab(RefVarTab(doc->var), NULL);
		PushVarTab(RefVarTab(Type_Document.vtab),
			LvalObj(&Type_Document, doc, &doc));
		iocpy_psub(doc->in, buf, c, NULL, 0);
		PopVarTab();
		PopVarTab();
		io_ctrl(doc->in, IOPP_EXPAND, save_expand);
		io_rewind(buf);
		save_in = doc->in;
		doc->in = buf;
		DocEval_std.func(doc);
		io_close(doc->in);
		doc->in = save_in;
	}
	else
	{
		io_getc(doc->in);
		Document_putc(c, doc);
	}
}


void DocEval_psub (Document_t *doc)
{
	int c;

	while ((c = io_getc(doc->in)) != EOF)
	{
		if	(c == '$')
			DocSubEval_psub(doc, c);
		else if	(c != '\\')
			Document_putc(c, doc);
		else if	(!DocSubEval_macro(doc, c, 1))
			return;
	}
}

