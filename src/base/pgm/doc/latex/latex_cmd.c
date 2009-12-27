/*
Steuerbefehle

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

#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <EFEU/patcmp.h>
#include <DocCtrl.h>
#include <LaTeX.h>
#include <efeudoc.h>
#include <ctype.h>

static char *cmd_break = "\\\\";
static char *cmd_newpage = "\\newpage";
static char *cmd_toc = "\\tableofcontents";
static char *cmd_lof = "\\listoffigures";
static char *cmd_lot = "\\listoftables";
static char *cmd_appendix = "\\appendix";
static char *cmd_item = "\\item";
static char *cmd_mark = "\\footnotemark[$1]";

static char *ref_label = "\\label";
static char *ref_std = "\\protect\\ref";
static char *ref_page = "\\protect\\pageref";
static char *ref_var = "\\protect\\vref";

static char *tab_sep = " &";
static char *tab_end = "\\\\";
static char *tab_hline = "\\hline";
static char *tab_cline = "\\cline{$1}";

static EfiVarDef cmd_tab[] = {
	{ "break",	&Type_str, &cmd_break },
	{ "newpage", &Type_str, &cmd_newpage },
	{ "tableofcontents", &Type_str, &cmd_toc },
	{ "listoffigures", &Type_str, &cmd_lof },
	{ "listoftables", &Type_str, &cmd_lot },
	{ "appendix", &Type_str, &cmd_appendix },
	{ "item", &Type_str, &cmd_item },
	{ "mark",	&Type_str, &cmd_mark },

	{ "label",	&Type_str, &ref_label },
	{ "ref",	&Type_str, &ref_std },
	{ "pageref",	&Type_str, &ref_page },
	{ "vref",	&Type_str, &ref_var },

	{ "tabsep", &Type_str, &tab_sep },
	{ "tabend", &Type_str, &tab_end },
	{ "hline", &Type_str, &tab_hline },
	{ "cline", &Type_str, &tab_cline },
};

static void print_var (IO *io, EfiVarDef *var, size_t dim)
{
	io_putc('\n', io);

	for (; dim-- > 0; var++)
		io_xprintf(io, "%s = %#s;\n", var->name, Val_str(var->data));
}


void LaTeX_ShowCmd (IO *io)
{
	print_var(io, cmd_tab, tabsize(cmd_tab));
}

void LaTeX_SetupCmd (void)
{
	AddVarDef(LocalVar, cmd_tab, tabsize(cmd_tab));
}

static void label_cmd (LaTeX *ltx, const char *cmd, const char *name)
{
	if	(name)
	{
		io_puts(cmd, ltx->out);
		io_putc('{', ltx->out);

		for (; *name != 0; name++)
		{
			if	(isalnum(*name) || strchr(".:/-", *name))
				io_putc(*name, ltx->out);
			else	io_xprintf(ltx->out, "[%02X]", *name);
		}

		io_putc('}', ltx->out);
	}
}

static int index_mark (int c)
{
	switch ((char) c)
	{
	case '!': case '@':
		return 1;
	default:
		return 0;
	}
}

static void put_index (LaTeX *ltx, va_list list)
{
	char *name = va_arg(list, char *);
	char *label = va_arg(list, char *);

	if	(!name || !label)	return;

	LaTeX_indexmode = 1;
	LaTeX_puts(ltx, lexsortkey(name, NULL));
	LaTeX_indexmode = 0;
	io_putc('@', ltx->out);

	for (; *label != 0; label++)
	{
		if	(index_mark(*label))
			io_putc('"', ltx->out);

		io_putc(*label, ltx->out);
	}
}

static void add_space (LaTeX *ltx, int space)
{
	io_putc(space, ltx->out);
	ltx->last = space;
	ltx->ignorespace = 1;
}

static void line_cmd (LaTeX *ltx, const char *cmd, char *arg)
{
	char *args[2];
	args[0] = NULL;
	args[1] = arg;
	LaTeX_newline(ltx);
	io_psubvec(ltx->out, cmd, 2, args);

	if	(cmd)
		add_space(ltx, '\n');
}


int LaTeX_cmd (void *drv, va_list list)
{
	LaTeX *ltx = drv;
	int cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_CMD_BREAK:
		io_puts(cmd_break, ltx->out);
		break;
	case DOC_CMD_NPAGE:
		line_cmd(ltx, cmd_newpage, NULL);
		break;
	case DOC_CMD_TOC:
		line_cmd(ltx, cmd_toc, NULL);
		break;
	case DOC_CMD_LOF:
		line_cmd(ltx, cmd_lof, NULL);
		break;
	case DOC_CMD_LOT:
		line_cmd(ltx, cmd_lot, NULL);
		break;
	case DOC_CMD_APP:
		line_cmd(ltx, cmd_appendix, va_arg(list, char *));
		break;
	case DOC_CMD_ITEM:
		line_cmd(ltx, cmd_item, NULL);
		break;
	case DOC_CMD_MARK:
		io_psubarg(ltx->out, cmd_mark, "nd", va_arg(list, int));
		break;
	case DOC_CMD_IDX:
		if	(ltx->last != '\n')
			io_puts("%\n", ltx->out);

		io_puts("\\index{", ltx->out);
		put_index(ltx, list);
		io_puts("}%", ltx->out);
		add_space(ltx, '\n');
		break;
	case DOC_CMD_TEX:
		io_puts(va_arg(list, char *), ltx->out);
		break;
	case DOC_REF_LABEL:
		label_cmd(ltx, ref_label, va_arg(list, char *));
		LaTeX_rem(ltx, NULL);
		break;
	case DOC_REF_STD:
		label_cmd(ltx, ref_std, va_arg(list, char *));
		break;
	case DOC_REF_PAGE:
		label_cmd(ltx, ref_page, va_arg(list, char *));
		break;
	case DOC_REF_VAR:
		label_cmd(ltx, ref_var, va_arg(list, char *));
		break;
	case DOC_TAB_BEG:
		break;
	case DOC_TAB_SEP:
		io_puts(tab_sep, ltx->out);
		add_space(ltx, ' ');
		break;
	case DOC_TAB_END:
		io_puts(tab_end, ltx->out);
		add_space(ltx, '\n');
		break;
	case DOC_TAB_HLINE:
		switch (va_arg(list, int))
		{
		default:
			line_cmd(ltx, tab_hline, NULL);
			/* FALLTHROUGH */
		case 1:
			line_cmd(ltx, tab_hline, NULL);
			/* FALLTHROUGH */
		case 0:
			break;
		}

		break;
	case DOC_TAB_CLINE:
		if	(va_arg(list, int))
		{
			char *p = mvsprintf("%d-%d", list);
			line_cmd(ltx, tab_cline, p);
			memfree(p);
		}

		break;
	default:
		return EOF;
	}

	return 0;
}
