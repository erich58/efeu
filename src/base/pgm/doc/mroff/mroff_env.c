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

#include <mroff.h>

#define	ATT_RM	"\\fR"
#define	ATT_IT	"\\fI"
#define	ATT_BF	"\\fB"
#define	ATT_TT	"\\fB"

/*	Ausgabefunktionen
*/

static char *Name = "NAME";
static char *FigName = "figure";
static char *TabName = "table";

static VarDef_t envpar[] = {
	{ "Name", &Type_str, &Name },
	{ "FigName", &Type_str, &FigName },
	{ "TabName", &Type_str, &TabName },
};

extern void mroff_envpar (VarTab_t *tab)
{
	AddVarDef(tab, envpar, tabsize(envpar));
}


static void put_env (mroff_t *mr, int flag, const char *cmd)
{
	if	(flag)
	{
		mroff_newline(mr);
		io_puts(cmd, mr->out);
	}
	else	mroff_cmdend(mr, NULL);
}


static void put_att (mroff_t *mr, int flag, char *att)
{
	if	(mr->copy)	att = NULL;

	if	(flag)		pushstack(&mr->s_att, mr->att);
	else			att = popstack(&mr->s_att, NULL);

	if	(att == mr->att)	;
	else if	(att)		io_puts(att, mr->out);
	else if	(mr->att)	io_puts(ATT_RM, mr->out);

	mr->att = att;
}

static void man_caption (mroff_t *mr, int flag)
{
	if	(flag)
	{
		mroff_newline(mr);
		io_puts(".SH ", mr->out);
		mroff_cmdend(mr, Name);
	}
}

/*	Umgebung beginnen/beenden
*/

int mroff_env (mroff_t *mr, int flag, va_list list)
{
	int cmd = va_arg(list, int);

	switch (cmd)
	{
/*	Absatzformen
*/
	case DOC_PAR_STD:

		if	(mr->nextpar)
			mroff_cmdline(mr, mr->nextpar);

		mr->nextpar = NULL;
		break;
	case DOC_PAR_TAG:
		if	(flag)
			mroff_cmdline(mr, ".TP");
		mr->item = flag;
		break;

/*	Gliederungsbefehle
*/
	case DOC_SEC_PART:
	case DOC_SEC_MCHAP:
	case DOC_SEC_CHAP:
	case DOC_SEC_SECT:

		if	(flag)
			mroff_cmdline(mr, ".TH");

		put_env(mr, flag, ".SH ");
		break;

	case DOC_ENV_MPAGE:

		if	(flag)
		{
			char *num = va_arg(list, char *);
			mroff_newline(mr);
			io_puts(".TH ", mr->out);
			io_puts(va_arg(list, char *), mr->out);
			io_putc(' ', mr->out);
			mroff_cmdend(mr, num);
			mroff_push(mr);
			mr->var.caption = man_caption;
		}
		else	mroff_pop(mr);

		break;

	case DOC_SEC_SSECT:
	case DOC_SEC_HEAD:
	case DOC_SEC_PARA:
		put_env(mr, flag, ".SH ");
		break;
	case DOC_SEC_SHEAD:
		put_env(mr, flag, ".SS ");
		break;
	case DOC_SEC_CAPT:
		if	(mr->var.caption)
			mr->var.caption(mr, flag);
		else	put_env(mr, flag, ".SS ");
		break;
	case DOC_SEC_SCAPT:
	case DOC_SEC_MARG:
	case DOC_SEC_NOTE:
		break;

/*	Attribute
*/
	case DOC_ATT_RM:	put_att(mr, flag, NULL); break;
	case DOC_ATT_BF:	put_att(mr, flag, ATT_BF); break;
	case DOC_ATT_IT:	put_att(mr, flag, ATT_IT); break;
	case DOC_ATT_TT:	put_att(mr, flag, ATT_TT); break;

/*	Anführungen
*/
	case DOC_QUOTE_SGL:	io_puts(flag ? "`" : "'", mr->out); break;
	case DOC_QUOTE_DBL:	io_puts(flag ? "\"" : "\"", mr->out); break;

/*	Verarbeitungsmodi
*/
	case DOC_MODE_COPY:

		if	(flag)	mroff_cbeg(mr, NULL);
		else		mroff_cend(mr, 0);

		break;
	case DOC_MODE_SKIP:
		mr->skip = flag;
		break;
	case DOC_MODE_MAN:
	case DOC_MODE_PLAIN:
		mr->put = flag ? DocDrv_plain : (DocDrvPut_t) mroff_putc;
		break;
	case DOC_MODE_TEX:
	case DOC_MODE_HTML:
		mroff_env (mr, flag, list);
		break;
	case DOC_MODE_VERB:
		mroff_newline(mr);
		mr->put = (DocDrvPut_t) (flag ? mroff_plain : mroff_putc);
		break;

/*	Listen
*/
	case DOC_LIST_ITEM:
	case DOC_LIST_ENUM:
	case DOC_LIST_DESC:
		mroff_cmdline(mr, flag ? ".PD 0" : ".PD");
		mr->nextpar = flag ? NULL : ".PP";
		break;
		
/*	Sonstige Umgebungen
*/
	case DOC_ENV_INTRO:
		if	(flag)
			mroff_cmdline(mr, ".SH INTRO");
		break;
	case DOC_ENV_QUOTE:
		mroff_newline(mr);
		io_puts(flag ? ".RS\n" : ".RE\n", mr->out);
		break;
	case DOC_ENV_CODE:
		put_att(mr, flag, ATT_TT);
		break;
	case DOC_ENV_FORMULA:
		break;
	case DOC_ENV_TABLE:
		mroff_cmdline(mr, ".SS Table");
		mr->skip = flag;
		break;
	case DOC_ENV_FIG:
		mroff_cmdline(mr, ".SS Figure");
		mr->skip = flag;
		break;
	default:
		return EOF;
	}

	return 0;
}
