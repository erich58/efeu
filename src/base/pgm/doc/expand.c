/*
Befehle und Makros exandieren

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

char *Doc_expand (Doc *doc, IO *in, int flag)
{
	StrBuf *buf;
	Doc save;
	
	buf = new_strbuf(0);
	save = *doc;
	doc->out = io_strbuf(buf);

	if	(doc->type && doc->type->filter)
		doc->out = doc->type->filter(doc->out);
	
	doc->stat = 1;
	doc->indent = 0;
	doc->nl = 1;
	memset(&doc->env, 0, sizeof(DocEnv));
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

char *Doc_xexpand (Doc *doc, IO *in)
{
	DocType *save;
	char *p;

	save = doc->type;
	doc->type = NULL;
	p = Doc_expand(doc, in, 1);
	doc->type = save;
	return p;
}
