/*	Kopierhilfsfunktionen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "efeudoc.h"
#include <ctype.h>

int DocVerb (io_t *in, io_t *out)
{
	int c, n;
	
	io_ctrl(out, DOC_BEG, DOC_ATT_TT);

	for (n = 0; (c = io_getc(in)) != EOF; n++)
	{
		if	(c == '|')
		{
			c = io_getc(in);

			if	(c != '|')
			{
				io_ungetc(c, in);
				break;
			}
		}

		if	(isspace(c))
			io_putc(' ', out);
		else	io_putc(c, out);
	}

	io_ctrl(out, DOC_END, DOC_ATT_TT);
	return n;
}

static void signum (io_t *in, io_t *out, int sig)
{
	if	(isdigit(io_peek(in)))
	{
		io_ctrl(out, DOC_BEG, DOC_ENV_FORMULA);
		io_putc(sig, out);
		io_ctrl(out, DOC_END, DOC_ENV_FORMULA);
	}
	else	io_putc(sig, out);
}

static void nbsp (io_t *out)
{
	if	(io_ctrl(out, DOC_SYM, "nbsp") == EOF)
		io_putc(' ', out);
}


void DocCopy (io_t *in, io_t *out)
{
	int c;
	int key;
	stack_t *stack;

	key = 0;
	stack = NULL;

	while ((c = io_getc(in)) != EOF)
	{
		if	(c == key && io_peek(in) == '>')
			continue;

		switch (c)
		{
		case '\\':	io_putc(io_getc(in), out); break;
		case '<':	key = DocMark_beg(&stack, out, in); break;
		case '>':	key = DocMark_end(&stack, out, key); break;
		case '|':	DocVerb(in, out); break;
		case '&':	DocSymbol(in, out); break;
		case '~':	nbsp(out); break;
		case '-':
		case '+':	signum(in, out, c); break;
		default:	io_putc(c, out); break;
		}
	}

	while (stack)
		key = DocMark_end(&stack, out, key);
}
