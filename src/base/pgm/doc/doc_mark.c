/*	Textauszeichnungen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "efeudoc.h"
#include <ctype.h>

int DocMark_type (int c, int def)
{
	switch (c)
	{
	case '\'':	return DOC_QUOTE_SGL;
	case '"':	return DOC_QUOTE_DBL;
	case '*':	return DOC_ATT_BF;
	case '=':	return DOC_ATT_RM;
	case '|':	return DOC_ATT_TT;
	case '/':	return DOC_ATT_IT;
	default:	return def;
	}
}

#define	mtype(x)	DocMark_type((x), DOC_ATT_IT)

int DocMark_beg (stack_t **stack, io_t *out, io_t *in)
{
	int key = io_getc(in);

	if	(DocMark_type(key, 0) == 0)
	{
		io_ungetc(key, in);
		key = 0;
	}

	io_ctrl(out, DOC_BEG, mtype(key));
	pushstack(stack, (void *) (size_t) key);
	return key;
}

int DocMark_end (stack_t **stack, io_t *out, int key)
{
	if	(*stack)
	{
		io_ctrl(out, DOC_END, mtype(key));
		popstack(stack, NULL);
		return *stack ? (int) (size_t) (*stack)->data : 0;
	}
	else	return 0;
}


/*	Dokumentmarkierung
*/


void Doc_begmark (Doc_t *doc, io_t *in)
{
	Doc_hmode(doc);
	doc->env.mark_key = DocMark_beg(&doc->env.mark, doc->out, in);
}

void Doc_endmark (Doc_t *doc)
{
	doc->env.mark_key = DocMark_end(&doc->env.mark, doc->out,
		doc->env.mark_key);
}

void Doc_nomark (Doc_t *doc)
{
	while (doc->env.mark)
		Doc_endmark(doc);
}
