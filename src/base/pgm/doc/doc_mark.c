/*
Textauszeichnungen

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

int DocMark_beg (Stack **stack, IO *out, IO *in)
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

int DocMark_end (Stack **stack, IO *out, int key)
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


void Doc_begmark (Doc *doc, IO *in)
{
	Doc_hmode(doc);
	doc->env.mark_key = DocMark_beg(&doc->env.mark, doc->out, in);
}

void Doc_endmark (Doc *doc)
{
	doc->env.mark_key = DocMark_end(&doc->env.mark, doc->out,
		doc->env.mark_key);
}

void Doc_nomark (Doc *doc)
{
	while (doc->env.mark)
		Doc_endmark(doc);
}
