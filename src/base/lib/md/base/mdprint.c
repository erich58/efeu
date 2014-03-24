/*
Multidimensionale Matrix standardisiert ausgeben

$Copyright (C) 1994, 2005 Erich Frühstück
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

#include <EFEU/printobj.h>
#include <EFEU/mdmat.h>
#include <EFEU/pctrl.h>
#include <EFEU/locale.h>
#include <EFEU/parsearg.h>

#define H_MAGIC		"##MDMAT   "
#define H_TITLE		"##Title   "
#define H_TYPE		"##Type    "
#define H_HEAD		"##Head    "
#define H_LOCALE	"##Locale  "
#define H_ROWS		"##Rows    "
#define H_COLUMNS	"##Columns "
#define H_AXIS		"##Axis"
#define H_INDEX		"##Index"
#define H_VERSION	"2.0"

#define	LLABEL	"."
#define	CLABEL	"."

#define	XMARK	MDXFLAG_MARK

static int head_flag = 2;
static int data_flag = 1;
static int mark_flag = 2;
static int zero_flag = 1;
static int label_width = 0;

static int cur_line = 0;
static int max_line = 0;
static int max_col = 0;
static int out_mode = 0;

extern int PrintFieldWidth;
extern int PrintFloatPrec;

static IO *set_pctrl(IO *io, const char *def)
{
	char *p, **list;
	IO *out;
	size_t dim;
	int i;

	out = NULL;
	dim = mstrsplit(def, "%s", &list);

	for (i = 0; i < dim; i++)
	{
		if	(list[i] == NULL)
			;
		else if	(strcmp("nodata", list[i]) == 0)
			data_flag = 0;
		else if	(strcmp("data", list[i]) == 0)
			data_flag = 1;
		else if	(strcmp("nohead", list[i]) == 0)
			head_flag = 0;
		else if	(strcmp("xhead", list[i]) == 0)
			head_flag = 3;
		else if	(strcmp("head", list[i]) == 0)
			head_flag = 2;
		else if	(strcmp("title", list[i]) == 0)
			head_flag = 1;
		else if	(strcmp("nomark", list[i]) == 0)
			mark_flag = 0;
		else if	(strcmp("mark", list[i]) == 0)
			mark_flag = 1;
		else if	(strcmp("nozero", list[i]) == 0)
			zero_flag = 0;
		else if	(strcmp("zero", list[i]) == 0)
			zero_flag = 1;
		else if	(patcmp("label=", list[i], &p))
			label_width = atoi(p);
		else if	(patcmp("landscape", list[i], NULL))
			pctrl_pgfmt = "a4r";
		else if	(patcmp("paper=", list[i], &p))
			pctrl_pgfmt = mstrcpy(p);
		else if	(patcmp("*pt", list[i], NULL))
			pctrl_fsize = atoi(list[i]);
		else if	(patcmp("font=", list[i], &p))
			pctrl_fsize = atoi(p);
		else if	(patcmp("mode=", list[i], &p) && !out)
		{
			if	(strcmp("binary", p) == 0)
			{
				out_mode = 1;
				out = io;
			}
			else if	(strcmp("map", p) == 0)
			{
				out_mode = 2;
				out = io;
			}
			else	out = io_pctrl(io, p);
		}
	}

	memfree(list);
	return out ? out : io_pctrl(io, NULL);
}

static void put_label(IO *io, const char *key, const char *arg)
{
	if	(arg)
	{
		io_ctrl(io, PCTRL_LEFT);
		io_puts(key, io);
		io_mputs(arg, io, ":");
		io_ctrl(io, PCTRL_LINE);
	}
}

static void show_axis(IO *io, const char *label, mdaxis *axis, int flag)
{
	char *delim;

	io_ctrl(io, PCTRL_LEFT);
	io_puts(label, io);

	for (delim = NULL; axis != NULL; axis = axis->next)
	{
		if	((axis->flags & XMARK) == flag)
		{
			io_puts(delim, io);
			io_puts(StrPool_get(axis->sbuf, axis->i_name), io);
			delim = " ";
		}
	}

	io_ctrl(io, PCTRL_LINE);
}


static void put_head (IO *io, char *head)
{
	char *p;

	for (; head; head = p)
	{
		if	((p = strchr(head, '\n')))
		{
			*p = 0;
			p++;
		}

		put_label(io, H_HEAD, head);
	}
}

static void show_header(IO *io, mdmat *md, int ext)
{
	char *head;

	put_label(io, H_MAGIC, H_VERSION);
	put_label(io, H_TITLE, StrPool_get(md->sbuf, md->i_name));

	if	((head = TypeHead(md->type)))
	{
		put_head(io, head);
		memfree(head);
	}

	io_ctrl(io, PCTRL_LEFT);
	io_puts(H_TYPE, io);
	ShowType(io, md->type);
	io_ctrl(io, PCTRL_LINE);

	if	(Locale.print)
		put_label(io, H_LOCALE, Locale.print->name);

	if	(ext)
	{
		StrPool *pool;
		mdaxis *axis;
		mdindex *idx;
		size_t n;

		pool = md->axis->sbuf;

		for (axis = md->axis; axis; axis = axis->next)
		{
			io_ctrl(io, PCTRL_LEFT);
			io_puts(H_AXIS, io);
			io_ctrl(io, PCTRL_LEFT);
			io_puts(StrPool_get(pool, axis->i_name), io);
			io_ctrl(io, PCTRL_LEFT);
			io_puts(StrPool_get(pool, axis->i_desc), io);
			io_ctrl(io, PCTRL_LINE);

			for (idx = axis->idx, n = axis->dim; n-- > 0; idx++)
			{
				io_ctrl(io, PCTRL_LEFT);
				io_puts(H_INDEX, io);
				io_ctrl(io, PCTRL_LEFT);
				io_puts(StrPool_get(pool, idx->i_name), io);
				io_ctrl(io, PCTRL_LEFT);
				io_puts(StrPool_get(pool, idx->i_desc), io);
				io_ctrl(io, PCTRL_LINE);
			}
		}
	}

	show_axis(io, H_ROWS, md->axis, 0);
	show_axis(io, H_COLUMNS, md->axis, XMARK);
}


static int vardim (EfiType *type, size_t dim)
{
	if	(dim)
	{
		return dim * vardim(type, 0);
	}
	else if	(type->dim)
	{
		return vardim(type->base, type->dim);
	}
	else if	(type->list)
	{
		EfiStruct *st;
		int n;

		for (n = 0, st = type->list; st != NULL; st = st->next)
			n += vardim(st->type, st->dim);

		return n;
	}
	else	return 1;
}

static int axis_dim(mdaxis *x)
{
	int i, k;

	for (i = k = 0; i < x->dim; i++)
		if	(!(x->idx[i].flags & MDFLAG_LOCK))
			k++;

	return k;
}

static int headdim(mdmat *md)
{
	mdaxis *x;
	int n;

	for (n = 1, x = md->axis; x != NULL; x = x->next)
		if (x->flags & XMARK) n *= axis_dim(x);

	return n * vardim(md->type, 0);
}

static void headline(IO *io, mdaxis *x, EfiType *type, const char *str);

static void headline2(IO *io, size_t dim, EfiType *type, const char *str)
{
	int i;
	char *p;

	for (i = 0; i < dim; i++)
	{
		p = msprintf("%s[%d]", str, i);
		headline(io, NULL, type, p);
		memfree(p);
	}
}


static void headline(IO *io, mdaxis *x, EfiType *type, const char *str)
{
	while (x != NULL && !(x->flags & XMARK))
		x = x->next;

	if	(x != NULL)
	{
		int i;
		char *p;

		for (i = 0; i < x->dim; i++)
		{
			if	(x->idx[i].flags & MDFLAG_LOCK)
				continue;

			p = mstrpaste(".", str,
				StrPool_get(x->sbuf, x->idx[i].i_name));
			headline(io, x->next, type, p);
			memfree(p);
		}
	}
	else if	(type->dim != 0)
	{
		headline2(io, type->dim, type->base, str);
	}
	else if	(type->list != NULL)
	{
		EfiStruct *st;
		char *p;

		for (st = type->list; st != NULL; st = st->next)
		{
			p = mstrpaste(".", str, st->name);

			if	(st->dim)
			{
				headline2(io, st->dim, st->type, p);
			}
			else	headline(io, NULL, st->type, p);

			memfree(p);
		}
	}
	else
	{
		io_ctrl(io, PCTRL_RIGHT);
		io_xprintf(io, "%*s", PrintFieldWidth, str ? str : CLABEL);
	}
}

static void put_header(IO *io, mdmat *md)
{
	int hd = headdim(md);

	io_ctrl(io, PCTRL_DATA, hd);

	if	(mark_flag == 0)	return;

	io_ctrl(io, PCTRL_HEAD);
	io_ctrl(io, PCTRL_LEFT);

	switch (mark_flag)
	{
	case 1:	io_xprintf(io, "#%-*d", max(0, label_width - 1), hd); break;
	case 2:	io_xprintf(io, "%-*d", label_width, hd); break;
	default: break;
	}

	headline(io, md->axis, md->type, NULL);
	io_ctrl(io, PCTRL_LINE);
	io_ctrl(io, PCTRL_EHEAD);
}

/*	Spalten durchwandern
*/

static void t_walk(IO *io, EfiType *type, size_t dim, char *ptr)
{
	if	(dim)
	{
		int i;

		for (i = 0; i < dim; i++)
		{
			t_walk(io, type, 0, ptr);
			ptr += type->size;
		}
	}
	else if	(type->dim)
	{
		t_walk(io, type->base, type->dim, ptr);
	}
	else if	(type->list)
	{
		EfiStruct *st;

		for (st = type->list; st != NULL; st = st->next)
			t_walk(io, st->type, st->dim, ptr + st->offset);
	}
	else if	(zero_flag || Obj2bool(ConstObj(type, ptr)))
	{
		io_ctrl(io, PCTRL_VALUE);
		ShowData(io, type, ptr);
	}
	else	io_ctrl(io, PCTRL_EMPTY);
}

static void c_walk(IO *io, mdaxis *x, EfiType *type, char *ptr)
{
	while (x != NULL && !(x->flags & XMARK))
		x = x->next;

	if	(x != NULL)
	{
		int i;

		for (i = 0; i < x->dim; i++)
		{
			if	(x->idx[i].flags & MDFLAG_LOCK)
				continue;

			c_walk(io, x->next, type, ptr + i * x->size);
		}
	}
	else	t_walk(io, type, 0, ptr);
}


/*	Zeilen durchwandern
*/

static void l_walk(IO *io, mdmat *md, mdaxis *x, const char *label, char *ptr)
{
	if	(io_err(io))	return;

	while (x != NULL && (x->flags & XMARK))
		x = x->next;

	if	(x != NULL)
	{
		int i;
		char *p;

		for (i = 0; i < x->dim; i++)
		{
			if	(x->idx[i].flags & MDFLAG_LOCK)
				continue;

			p = mstrpaste(".", label,
				StrPool_get(x->sbuf, x->idx[i].i_name));
			l_walk(io, md, x->next, p, ptr + i * x->size);
			memfree(p);
		}
	}
	else
	{
		if	(max_line > 0 && io_ctrl(io, PCTRL_YPOS) >= max_line)
		{
			io_ctrl(io, PCTRL_EDATA);
			io_ctrl(io, PCTRL_PAGE);
			put_header(io, md);
			cur_line = 0;
		}

		io_ctrl(io, PCTRL_LEFT);
		io_xprintf(io, "%-*s", label_width, label ? label : LLABEL);
		c_walk(io, md->axis, md->type, ptr);
		io_ctrl(io, PCTRL_LINE);
		cur_line++;
	}
}


static void show_data(IO *io, mdmat *md)
{
	if	(io_err(io))	return;
	
	put_header(io, md);
	cur_line = 0;
	max_line = io_ctrl(io, PCTRL_LINES);
	max_col = io_ctrl(io, PCTRL_COLUMNS);
	l_walk(io, md, md->axis, NULL, md->data);
	io_ctrl(io, PCTRL_EDATA);
}

void md_print (IO *io, mdmat *md, const char *def)
{
	if	(md == NULL)	return;

	io = set_pctrl(io, def);

	switch (out_mode)
	{
	case 2:
		md_putmap(md, io);
		io_close(io);
		return;
	case 1:
		md_save(io, md, 0);
		io_close(io);
		return;
	}

	switch (head_flag)
	{
	case  3:
		show_header(io, md, 1); break;
	case  2:
		show_header(io, md, 0); break;
	case  1:
		put_label(io, NULL, StrPool_get(md->sbuf, md->i_name));
		break;
	default:
		break;
	}

	if	(data_flag)	show_data(io, md);

	io_close(io);
}
