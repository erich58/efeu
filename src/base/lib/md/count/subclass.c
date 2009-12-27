/*
Unterselektionsgruppe definieren

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <ctype.h>

typedef struct {
	MDCLASS_VAR;
	MdClass *main;
	int *idx;
} MdSubClass;


static int subclassify (MdClass *data, const void *ptr)
{
	MdSubClass *sel = (MdSubClass *) data;
	int n = MdClassify(sel->main, ptr);

	if	(n >= 0 && n < sel->main->dim)
	{
		return sel->idx[n];
	}
	else	return sel->dim;
}

static int scan_name (IO *in, StrBuf *buf, const char *delim)
{
	int c;
	int depth;

	depth = 0;
	sb_setpos(buf, 0);

	while ((c = io_getc(in)) != EOF)
	{
		if	(c == '\\')
		{
			if	(c != '\\' && c != '[' && c != ']' &&
					 !isspace(c) && !strchr(delim, c))
				sb_putc('\\', buf);
		}
		else if	(c == '[')
		{
			depth++;
		}
		else if	(c == ']')
		{
			if	(depth >= 0)	depth--;
		}
		else if	(!depth && (isspace(c) || strchr(delim, c)))
		{
			break;
		}

		sb_putc(c, buf);
	}

	sb_putc(0, buf);
	return c;
}

static VECBUF(buf_label, 64, sizeof(Label));

/*	Unterselektion generieren
*/

MdClass *md_subclass (MdClass *base, const char *def)
{
	int c;
	int i;
	char *p;
	char **list;
	int dim;
	MdSubClass *sub;
	Label *label;
	StrBuf *buf;
	IO *io;

	if	(def == NULL)	return base;

	sub = memalloc(sizeof(MdSubClass) + base->dim * sizeof(int));
	sub->name = NULL;
	sub->dim = 0;
	sub->label = NULL;
	sub->classify = subclassify;
	sub->idx = (int *) (sub + 1);
	sub->main = base;

	for (i = 0; i < base->dim; i++)
		sub->idx[i] = base->dim;

	io = io_cstr(def);
	buf_label.used = 0;
	buf = new_strbuf(0);

	while ((c = io_eat(io, "%s;")) != EOF)
	{
		label = NULL;
		c = scan_name(io, buf, "=;");

		if	(c == '=')
		{
			label = vb_next(&buf_label);
			label->name = sb_memcpy(buf);
			label->desc = NULL;
			sub->dim++;
			c = scan_name(io, buf, ";");
		}

		dim = strsplit((char *) buf->data, ",", &list);

		for (i = 0; i < base->dim; i++)
		{
			if	(sub->idx[i] < base->dim)
			{
				continue;
			}
			else if	(!patselect(base->label[i].name, list, dim))
			{
				continue;
			}
			else if	(label)
			{
				sub->idx[i] = sub->dim - 1;
				p = label->desc;
				label->desc = mstrpaste("; ", p,
					base->label[i].desc);
				memfree(p);
			}
			else
			{
				label = vb_next(&buf_label);
				label->name = mstrcpy(base->label[i].name);
				label->desc = mstrcpy(base->label[i].desc);
				sub->idx[i] = sub->dim++;
			}
		}

		memfree(list);
	}

	io_close(io);
	del_strbuf(buf);
	sub->label = memalloc(sub->dim * sizeof(Label));
	memcpy(sub->label, buf_label.data, sub->dim * sizeof(Label));
	return (MdClass *) sub;
}
