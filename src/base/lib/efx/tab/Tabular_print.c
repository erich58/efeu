/*
Tabellenausgabe

$Copyright (C) 2008 Erich Frühstück
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

#include <EFEU/Tabular.h>

#if	0
static void row_info (Tabular *tab, IO *out, TabularRow *row)
{
	static TabularRow *last = NULL;

	if	(row == &tab->buf.row)
		io_printf(out, "** ");
	else	io_printf(out, "%2d ", row - (TabularRow *) tab->row.data);

	if	(row && row != last)
		io_printf(out, "%d %d ", row->hline, row->height);
	else	io_printf(out, "%4s", "");

	last = row;
}
#else
#define row_info(tab,out,row)
#endif

static void put_border(TabularLinestyle ltype, IO *out, int height)
{
	switch (ltype)
	{
	case TabularLinestyle_none:	break;
	case TabularLinestyle_single:	io_putc(height ? '|' : '+', out); break;
	case TabularLinestyle_double:	io_putc('#', out); break;
	}
}

static void put_hline (Tabular *tab, IO *out, TabularRow *row)
{
	char *symbol;
	TabularCol *col;
	size_t n;

	switch (row->hline)
	{
	case TabularLinestyle_none:	return;
	case TabularLinestyle_single:	symbol = "-+#"; break;
	case TabularLinestyle_double:	symbol = "=##"; break;
	}

	row_info(tab, out, row);
	io_nputc(' ', out, tab->margin);
	col = tab->col.data;

	for (n = 0; n < tab->col.used; n++)
	{
		io_putc(symbol[col[n].left_border], out);
		io_nputc(symbol[0], out, col[n].left_margin +
			col[n].width + col[n].right_margin - 2);
		io_putc(symbol[col[n].right_border], out);
	}

	io_putc('\n', out);
}

static void put_cline (Tabular *tab, IO *out, int hline, int pos, int dim)
{
	char *symbol;
	TabularCol *col;
	size_t n;

	switch (hline)
	{
	case TabularLinestyle_none:	symbol = " +#"; break;
	case TabularLinestyle_single:	symbol = "-+#"; break;
	case TabularLinestyle_double:	symbol = "=##"; break;
	}

	col = tab->col.data;

	for (n = 0; n < dim; n++)
	{
		io_putc(symbol[col[pos].left_border], out);
		io_nputc(symbol[0], out, col[pos].left_margin +
			col[pos].width + col[pos].right_margin - 2);
		io_putc(symbol[col[pos].right_border], out);
		pos++;
	}
}

static void empty_col (Tabular *tab, IO *out, int cpos, int height)
{
	TabularCol *col = Tabular_getcol(tab, cpos);
	int n = col->left_margin + col->width + col->right_margin;

	if	(col->left_border)	n--;
	if	(col->right_border)	n--;

	put_border(col->left_border, out, height);
	io_nputc(' ', out, n);
	put_border(col->right_border, out, height);
}

static void complete_line (Tabular *tab, IO *out, int cpos, int height)
{
	while (cpos < tab->col.used)
	{
		if	(cpos == 0)
			io_nputc(' ', out, tab->margin);

		empty_col(tab, out, cpos, height);
		cpos++;
	}

	io_putc('\n', out);
}

static int cmp_entry (const void *opaque_a, const void *opaque_b)
{
	const TabularEntry *a = opaque_a;
	const TabularEntry *b = opaque_b;

	if	(a->lnum < b->lnum)	return -1;
	if	(a->lnum > b->lnum)	return 1;
	if	(a->cpos < b->cpos)	return -1;
	if	(a->cpos > b->cpos)	return 1;
	if	(a->cdim < b->cdim)	return -1;
	if	(a->cdim > b->cdim)	return 1;

	return 0;
}

void Tabular_print (Tabular *tab, IO *out)
{
	TabularEntry *entry;
	TabularCol *col;
	TabularRow *row;
	char *text;
	int n, k;
	int lnum, cpos;
	int width;

	if	(!tab || !out)	return;

	text = (char *) tab->text.data;
	vb_qsort(&tab->entry, cmp_entry);

/*	Breitenbestimmung für Mehrfachspalten
*/
	for (entry = tab->entry.data, n = tab->entry.used; n--; entry++)
	{
		if	(entry->cdim < 2)	continue;

		col = Tabular_getcol(tab, entry->cpos);
		width = (int) entry->col.width - (int) col->width;

		for (k = 1; k < entry->cdim; k++)
		{
			width -= col->right_margin;
			col = Tabular_getcol(tab, entry->cpos + k);
			width -= col->left_margin + col->width;
		}

		if	(width <= 0)	continue;

		for (k = 0; k < entry->cdim; k++)
		{
			int z = 0.5 + (double) width / (entry->cdim - k);
			col = Tabular_getcol(tab, entry->cpos + k);
			col->width += z;
			width -= z;
		}
	}

#if	0
	io_nputc(' ', out, tab->margin);

	for (n = 0; n < tab->col.used; n++)
	{
		put_border(col[n].left_border, out, 1);
		io_nputc(' ', out, col[n].left_margin -
			(col[n].left_border ? 1 : 0));
		io_nputc('X', out, col[n].width);
		io_nputc(' ', out, col[n].right_margin -
			(col[n].right_border ? 1 : 0));
		put_border(col[n].right_border, out, 1);
	}

	io_putc('\n', out);
#endif

	text = (char *) tab->text.data;

	lnum = 0;
	row = Tabular_getrow(tab, lnum);
	put_hline(tab, out, row);
	cpos = 0;

	for (entry = tab->entry.data, n = tab->entry.used; n--; entry++)
	{
		while	(lnum < entry->lnum)
		{
			complete_line(tab, out, cpos, row->height);
			lnum++;
			row = Tabular_getrow(tab, lnum);
			put_hline(tab, out, row);
			cpos = 0;
		}

		if	(cpos == 0)
		{
			row_info(tab, out, row);
			io_nputc(' ', out, tab->margin);
		}

		while (cpos < entry->cpos)
		{
			empty_col(tab, out, cpos, row->height);
			cpos++;
		}

		if	(row->height == 0)
		{
			put_cline(tab, out, entry->row.hline,
				entry->cpos, entry->cdim);
			cpos += entry->cdim;
			continue;
		}

		col = Tabular_getcol(tab, entry->cpos);
		put_border(entry->col.left_border, out, 1);
		io_nputc(' ', out, col->left_margin -
			(entry->col.left_border ? 1 : 0));

		width = col->width;

		for (k = 1; k < entry->cdim; k++)
		{
			width += col->right_margin;
			col = Tabular_getcol(tab, entry->cpos + k);
			width += col->left_margin + col->width;
		}

		switch (entry->col.align)
		{
		case TabularAlign_right:
			k = width - entry->col.width;
			break;
		case TabularAlign_center:
			k = (width - entry->col.width) / 2;
			break;
		default:
			k = 0;
			break;
		}

		width -= io_nputc(' ', out, k);
		io_puts(text + entry->i_text, out);
		io_nputc(' ', out, width - entry->col.width);
		io_nputc(' ', out, col->right_margin -
			(entry->col.right_border ? 1 : 0));
		put_border(entry->col.right_border, out, 1);
		cpos += entry->cdim;
	}

	complete_line(tab, out, cpos, row->height);
	put_hline(tab, out, &tab->buf.row);
}
