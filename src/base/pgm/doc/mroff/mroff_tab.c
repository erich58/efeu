/*
Tabellenumgebung

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

#include <mroff.h>

static SB_DECL(tab_buf, 0);
static SB_DECL(tab_col, 0);
static VECBUF(colgrp, 32, sizeof(int));
static IO *save_out = NULL;
static int cpos = 0;
static char *cdef = NULL;

static char *get_col (void)
{
	int *p = vb_data(&colgrp, cpos);
	cpos++;
	return p ? (char *) tab_col.data + *p : "l";
}

static void col_def (const char *def)
{
	int *p;
	
	colgrp.used = 0;

	if	(!def)	return;

	p = vb_next(&colgrp);
	*p = tab_col.pos;

	for (; *def != 0; def++)
	{
		int col = *def;

		switch (*def)
		{
		case 'X':
			col = 'l';
			/* FALLTHROUGH */
		case 'l':
		case 'c':
		case 'r':
			if	(!p)
			{
				sb_putc(0, &tab_col);
				p = vb_next(&colgrp);
				*p = tab_col.pos;
			}

			sb_putc(col, &tab_col);
			p = NULL;
			break;
		case '|':
			sb_putc(col, &tab_col);
			break;
		default:
			break;
		}
	}

	sb_putc(0, &tab_col);
}

void mroff_tab_beg (ManRoff *mr, const char *opt, const char *def)
{
	mroff_newline(mr);
	col_def(def);
	io_puts(".TS\n", mr->out);

	save_out = mr->out;
	sb_trunc(&tab_buf);
	mr->out = io_strbuf(&tab_buf);
}

void mroff_tab_end (ManRoff *mr)
{
	io_close(mr->out);
	mr->out = save_out;
	io_puts(".\n", mr->out);
	io_puts(".\\\" Tabellendaten\n", mr->out);
	io_puts(sb_nul(&tab_buf), mr->out);
	save_out = NULL;
	mroff_cmdline(mr, ".TE");
}

void mroff_tab_mcol (ManRoff *mr, int cdim, const char *def)
{
	int pos = tab_col.pos;
	int flag = 1;

	if	(!def)	def = "l";

	for (; *def; def++)
	{
		int col = *def;

		switch (*def)
		{
		case 'X':
			col = 'l';
			/* FALLTHROUGH */
		case 'l':
		case 'r':
		case 'c':
			if	(flag)
			{
				sb_putc(col, &tab_col);
				sb_nputc('s', &tab_col, cdim - 1);
				flag = 0;
			}
			break;
		case '|':
			sb_putc(col, &tab_col);
			break;
		}
	}

	sb_putc(0, &tab_col);
	tab_col.pos = pos;
	sb_sync(&tab_col);
	cdef = (char *) tab_col.data + pos;
	cpos += cdim - 1;
}

void mroff_tab_hline (ManRoff *mr, int n)
{
	mroff_cmdline(mr, n > 1 ? "=" : "_");
}

void mroff_tab_cline (ManRoff *mr, int n, int p1, int p2)
{
	;
}

void mroff_tab_sep (ManRoff *mr)
{
	io_puts(cdef, save_out);
	io_putc('\t', mr->out);
	cdef = get_col();
}

void mroff_tab_begrow (ManRoff *mr)
{
	mroff_newline(mr);
	cpos = 0;
	cdef = get_col();
}

void mroff_tab_endrow (ManRoff *mr)
{
	io_puts(cdef, save_out);
	io_putc('\n', save_out);
	mroff_newline(mr);
}
