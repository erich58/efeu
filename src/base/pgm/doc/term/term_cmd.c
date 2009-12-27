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
#include <DocCtrl.h>
#include <term.h>
#include <efeudoc.h>

int term_cmd (void *drv, va_list list)
{
	Term *trm = drv;
	int cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_CMD_TOC:
		term_newline(trm, 1);
		term_att(trm, 1, term_par.bf);
		term_string(trm, "Inhalt");
		term_att(trm, 1, term_par.bf);
		term_newline(trm, 1);
		break;
	case DOC_CMD_BREAK:
		term_newline(trm, 0);
		break;
	case DOC_CMD_ITEM:
		term_newline(trm, 0);
		trm->var.margin -= TERM_INDENT;
		term_putc(trm, '*');
		trm->var.margin += TERM_INDENT;
		break;
	case DOC_TAB_BEG:
		Tabular_newline(trm->tab);
		break;
	case DOC_TAB_SEP:
		if	(trm->tab)
			trm->tab->buf.col.width = trm->col;

		trm->col = 0;
		trm->mode = 0;
		trm->space = 0;
		Tabular_entry(trm->tab);
		break;
	case DOC_TAB_END:
		if	(trm->tab)
			trm->tab->buf.col.width = trm->col;

		trm->col = 0;
		trm->mode = 0;
		trm->space = 0;
		Tabular_endline(trm->tab);
		break;
	case DOC_TAB_HLINE:
		Tabular_hline(trm->tab, va_arg(list, int));
		break;
	case DOC_TAB_CLINE:
		{
			int n = va_arg(list, int);
			int p1 = va_arg(list, int);
			int p2 = va_arg(list, int);
			Tabular_cline(trm->tab, n, p1, p2);
		}
		break;
	default:
		return EOF;
	}

	return 0;
}
