/*	Dokumentfunktion: Vorzeichen und Striche
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

void DocSubEval_sig (Document_t *doc, int sig)
{
	int c = io_peek(doc->in);

	if	(c == '-' && sig == '-')
	{
		io_getc(doc->in);

		if	(io_peek(doc->in) == '-')
		{
			io_getc(doc->in);
			Document_symbol(doc, "emdash");
		}
		else	Document_symbol(doc, "endash");
	}
	else if	(isdigit(c) || c == '.' || c == ',')
	{
		Document_symbol(doc, sig == '+' ? "plus" : "minus");
	}
	else	Document_putc(sig, doc);
}
