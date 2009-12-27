/*
Umgebungswechsel

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

#include <term.h>

static void term_sec (Term *trm, int flag)
{
	if	(flag)
	{
		term_newline(trm, 1);
		term_push(trm);
		trm->var.margin = 0;
		term_att(trm, 1, term_par.bf);
	}
	else
	{
		term_att(trm, 1, term_par.rm);
		term_pop(trm);
		term_newline(trm, 0);
	}
}

static void man_caption (Term *trm, int flag)
{
	if	(flag)
	{
		unsigned save = trm->var.margin;
		term_newline(trm, 1);
		trm->var.margin = 0;
		term_att(trm, 1, term_par.bf);
		term_string(trm, term_par.Name);
		term_att(trm, 1, term_par.rm);
		trm->var.margin = save;
		term_newline(trm, 0);
	}
}

int term_env (void *drv, int flag, va_list list)
{
	Term *trm = drv;
	int cmd = va_arg(list, int);

	switch (cmd)
	{
/*	Absatzformen
*/
	case DOC_PAR_STD:

		if	(flag)
		{
			trm->hang = trm->hangpar ? TERM_INDENT : 0;
		}
		else
		{
			term_newline(trm, 1);
			trm->var.margin -= trm->hangpar ? TERM_INDENT : 0;
		}

		break;

/*	Gliederungsbefehle
*/
	case DOC_SEC_PART:
	case DOC_SEC_MCHAP:
	case DOC_SEC_CHAP:
	case DOC_SEC_SECT:
	case DOC_SEC_SSECT:
	case DOC_SEC_HEAD:
	case DOC_SEC_PARA:
	case DOC_SEC_SHEAD:
	case DOC_SEC_MARG:
	case DOC_SEC_NOTE:
		term_sec(trm, flag);
		break;
	case DOC_SEC_CAPT:

		if	(trm->var.caption)
			trm->var.caption(trm, flag);
		else	term_sec(trm, flag);

		break;
	case DOC_SEC_SCAPT:
		break;
	case DOC_ENV_MPAGE:

		if	(flag)
		{
			char *num = va_arg(list, char *);
			term_newline(trm, 1);
			term_string(trm, va_arg(list, char *));
			trm->put(trm, '(');
			term_string(trm, num);
			trm->put(trm, ')');
			term_newline(trm, 1);
			term_push(trm);
			trm->var.caption = man_caption;
		}
		else	term_pop(trm);

		break;
/*	Attribute und Anführungen
*/
	case DOC_ATT_RM:	term_att(trm, flag, NULL); break;
	case DOC_ATT_BF:	term_att(trm, flag, term_par.bf); break;
	case DOC_ATT_IT:	term_att(trm, flag, term_par.it); break;
	case DOC_ATT_TT:	term_att(trm, flag, term_par.tt); break;
	case DOC_QUOTE_SGL:	term_putc(trm, '\''); break;
	case DOC_QUOTE_DBL:	term_putc(trm, '"'); break;

/*	Verarbeitungsmodi
*/
	case DOC_MODE_COPY:
		trm->put = flag ? DocDrv_plain : term_putc;
		break;
	case DOC_MODE_PLAIN:
		break;
	case DOC_MODE_VERB:
		term_newline(trm, 0);
		term_att(trm, flag, term_par.tt);
		trm->put = (flag ? term_verb : term_putc);
		break;
	case DOC_MODE_SKIP:
	case DOC_MODE_MAN:
	case DOC_MODE_TEX:
	case DOC_MODE_HTML:
		trm->skip = flag;
		break;

/*	Listen
*/
	case DOC_LIST_ITEM:
	case DOC_LIST_ENUM:
	case DOC_LIST_DESC:
		term_newline(trm, 0);
		trm->var.margin += flag ? TERM_INDENT : -TERM_INDENT;
		break;
		
/*	Sonstige Umgebungen
*/
	case DOC_ENV_INTRO:
		if	(flag)
		{
			term_newline(trm, 1);
			term_att(trm, 1, term_par.bf);
			term_string(trm, "Vorwort");
			term_att(trm, 0, term_par.bf);
			term_newline(trm, 1);
		}
		break;
	case DOC_ENV_QUOTE:
		term_newline(trm, 0);
		trm->var.margin += flag ? TERM_INDENT : -TERM_INDENT;
		break;
	case DOC_ENV_HANG:
		trm->hangpar = flag;
		break;
	case DOC_ENV_CODE:
		term_att(trm, flag, term_par.tt);
		break;
	case DOC_ENV_FORMULA:
		break;
	case DOC_PAR_TAG:
		if	(flag)
		{
			term_newline(trm, 0);
			term_att(trm, 1, term_par.bf);
			trm->var.margin -= TERM_INDENT;
		}
		else
		{
			term_att(trm, 0, term_par.bf);
			trm->var.margin += TERM_INDENT;

			if	(trm->col >= trm->var.margin)
				term_newline(trm, 0);
		}
		break;
	case DOC_ENV_TABLE:
	case DOC_ENV_FIG:
		trm->skip = flag;
		break;
	case DOC_ENV_TAB:
		if	(flag)
		{
			char *opt = va_arg(list, char *);
			char *arg = va_arg(list, char *);
			rd_deref(trm->tab);
			trm->tab = NewTabular(arg, opt);
			trm->tab->margin = trm->var.margin;
			term_newline(trm, 0);
			term_push(trm);
			trm->save_out = trm->out;
			trm->out = trm->tab->io;
			trm->var.margin = 0;
		}
		else
		{
			trm->out = trm->save_out;
			trm->save_out = NULL;
			Tabular_print(trm->tab, trm->out);
			rd_deref(trm->tab);
			trm->tab = NULL;
			io_putc('\n', trm->out);
			term_pop(trm);
			term_newline(trm, 0);
		}
		break;
	case DOC_ENV_MCOL:
		if	(flag)
		{
			char *def = va_arg(list, char *);
			int cdim = va_arg(list, int);
			Tabular_multicol(trm->tab, cdim, def);
		}
		break;
	default:
		return EOF;
	}

	return 0;
}
