/*
Tabellenkonstruktion

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

#define	BS_TEXT		1000	/* Buffergröße für Text */
#define	BS_COL		10	/* Buffergröße für Spalten */
#define	BS_ROW		50	/* Buffergröße für Reihen */
#define	BS_ENTRY	250	/* Buffergröße für Einträge */

static void set_entry(Tabular *tab);

static char *tab_ident (const void *data)
{
	const Tabular *tab = data;
	StrBuf *sb = sb_acquire();
	IO *out = io_strbuf(sb);
	sb_printf(sb, "Tabular: size=%d, cols=%d, entry=%d\n",
		tab->text.pos, tab->col.used, tab->entry.used);
	Tabular_print((Tabular *) tab, out);
	io_close(out);
	return sb_cpyrelease(sb);
}

static void tab_clean (void *data)
{
	Tabular *tab = data;

	io_close(tab->io);
	sb_free(&tab->text);
	vb_free(&tab->col);
	vb_free(&tab->row);
	vb_free(&tab->entry);
	memfree(tab);
}

static RefType Tabular_reftype = REFTYPE_INIT("Tabular", tab_ident, tab_clean);

Tabular *NewTabular (const char *coldef, const char *opt)
{
	Tabular *tab = memalloc(sizeof *tab);

	sb_init(&tab->text, BS_TEXT);
	vb_init(&tab->col, BS_COL, sizeof(TabularCol));
	vb_init(&tab->row, BS_ROW, sizeof(TabularRow));
	vb_init(&tab->entry, BS_ENTRY, sizeof(TabularEntry));
	sb_puts(opt, &tab->text);
	sb_putc(0, &tab->text);
	Tabular_column(tab, coldef);
	tab->io = io_strbuf(&tab->text);

	tab->buf.i_text = tab->text.pos;
	tab->buf.lnum = 0;
	tab->buf.cpos = 0;
	set_entry(tab);
	return rd_init(&Tabular_reftype, tab);
}

static TabularCol *new_column (Tabular *tab, TabularAlign align)
{
	TabularCol *col = vb_next(&tab->col);
	col->align = align;
	col->left_border = TabularLinestyle_none;
	col->left_margin = 0;
	col->right_margin = 0;
	col->right_border = TabularLinestyle_none;
	col->width = 0;
	return col;
}

static void col_max(TabularCol *base, TabularCol *current)
{
	if	(base->left_margin < current->left_margin)
		base->left_margin = current->left_margin;

	if	(base->width < current->width)
		base->width = current->width;

	if	(base->right_margin < current->right_margin)
		base->right_margin = current->right_margin;
}

static void row_max(TabularRow *base, TabularRow *current)
{
	if	(base->top_margin < current->top_margin)
		base->top_margin = current->top_margin;

	if	(base->height < current->height)
		base->height = current->height;
}

static void save_entry (Tabular *tab)
{
	if	(tab->buf.i_text && tab->buf.i_text < tab->text.pos)
	{
		sb_putc(0, &tab->text);
		tab->buf.row.height = 1;
	}

	if	(tab->row.used <= tab->buf.lnum)
		vb_append(&tab->row, &tab->buf.row, 1);
	else	row_max(Tabular_getrow(tab, tab->buf.lnum), &tab->buf.row);

	if	(tab->buf.cdim == 1)
		col_max(Tabular_getcol(tab, tab->buf.cpos), &tab->buf.col);

	vb_append(&tab->entry, &tab->buf, 1);
}

static void set_entry(Tabular *tab)
{
	TabularCol *col;

	col = Tabular_getcol(tab, tab->buf.cpos);
	tab->buf.col = *col;
	tab->buf.col.width = 0;
	tab->buf.cdim = 1;
	tab->buf.row.height = 0;
	tab->buf.row.hline = 0;
	tab->buf.row.top_margin = 0;
	tab->buf.i_text = tab->text.pos;
}

void Tabular_multicol (Tabular *tab, int cdim, const char *def)
{
	if	(!tab || !def)	return;

	tab->buf.cdim = cdim;
	memset(&tab->buf.col, 0, sizeof tab->buf.col);
	tab->buf.col.left_border = TabularLinestyle_none;
	tab->buf.col.right_border = TabularLinestyle_none;

	for (; *def == '|'; def++)
	{
		if	(tab->buf.col.left_border < TabularLinestyle_double)
			tab->buf.col.left_border++;
	}

	for (; *def; def++)
	{
		switch (*def)
		{
		case 'p':
		case 'm':
		case 'b':
			tab->buf.col.align = TabularAlign_left;
			break;
		case 'r':
			tab->buf.col.align = TabularAlign_right;
			break;
		case 'X':
		case 'l':
			tab->buf.col.align = TabularAlign_left;
			break;
		case 'c':
			tab->buf.col.align = TabularAlign_center;
			break;
		case '|':
			if	(tab->buf.col.right_border <
					TabularLinestyle_double)
				tab->buf.col.right_border++;
			break;
		default:
			break;
		}
	}

	tab->buf.col.left_margin = tab->buf.col.left_border ? 2 : 1;
	tab->buf.col.right_margin = tab->buf.col.right_border ? 2 : 1;
}

void Tabular_column (Tabular *tab, const char *def)
{
	TabularCol buf, *col;
	size_t n;

	if	(!tab || !def)
		return;

	memset(&buf, 0, sizeof buf);

	for (col = &buf; *def; def++)
	{
		switch (*def)
		{
		case 'r':
			col = new_column(tab, TabularAlign_right);
			break;
		case 'X':
		case 'l':
			col = new_column(tab, TabularAlign_left);
			break;
		case 'c':
			col = new_column(tab, TabularAlign_center);
			break;
		case '|':
			if	(col->right_border < TabularLinestyle_double)
				col->right_border++;
			break;
		default:
			break;
		}
	}

	if	(!tab->col.used)
		new_column(tab, 0);

	col = tab->col.data;

	if	(buf.right_border)
		col->left_border = buf.right_border;

	for (n = 0; n < tab->col.used; n++)
	{
		col[n].left_margin = col[n].left_border ? 2 : 1;
		col[n].right_margin = col[n].right_border ? 2 : 1;
	}
}

void Tabular_newline (Tabular *tab)
{
	if	(tab && tab->buf.cpos)
	{
		tab->buf.lnum++;
		tab->buf.cpos = 0;
		set_entry(tab);
	}
}

void Tabular_endline (Tabular *tab)
{
	if	(tab)
	{
		save_entry(tab);
		tab->buf.lnum++;
		tab->buf.cpos = 0;
		set_entry(tab);
	}
}

void Tabular_entry (Tabular *tab)
{
	if	(tab)
	{
		save_entry(tab);
		tab->buf.cpos += tab->buf.cdim;
		set_entry(tab);
	}
}

void Tabular_hline (Tabular *tab, int n)
{
	if	(tab)
	{
		tab->buf.row.hline += n;

		if	(tab->buf.row.hline > TabularLinestyle_double)
			tab->buf.row.hline = TabularLinestyle_double;
	}
}

void Tabular_cline (Tabular *tab, int n, int p1, int p2)
{
	if	(tab)
	{
		TabularCol *col = Tabular_getcol(tab, p1 - 1);

		if	(n > TabularLinestyle_double)
			n = TabularLinestyle_double;

		if	(tab->row.used <= tab->buf.lnum)
			vb_append(&tab->row, &tab->buf.row, 1);

		memset(&tab->buf.row, 0, sizeof tab->buf.row);
		tab->buf.row.hline = n;
		tab->buf.row.height = 0;
		tab->buf.col = *col;
		tab->buf.col.width = 0;
		tab->buf.cpos = p1 - 1;
		tab->buf.cdim = p2 - p1 + 1;
		tab->buf.i_text = 0;
		vb_append(&tab->entry, &tab->buf, 1);

		tab->buf.cpos = p2 - 1;
		set_entry(tab);
	}
}
