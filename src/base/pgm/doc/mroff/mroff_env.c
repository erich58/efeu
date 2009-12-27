/*	Umgebungswechsel
	(c) 1999 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

#include <mroff.h>

#define	ATT_RM	"\\fR"
#define	ATT_IT	"\\fI"
#define	ATT_BF	"\\fB"
#define	ATT_TT	"\\fB"

/*	Ausgabefunktionen
*/

#if	0
static void sec_beg (mroff_t *mr, char *secnum)
{
	mroff_newline(mr);
	pushstack(&mr->stack, secnum);
	io_puts(".TH ", mr->out);
}
#endif

static void sec_end (mroff_t *mr)
{
	char *p = popstack(&mr->stack, 0);
	io_putc(' ', mr->out);
	mroff_cmdend(mr, p);
	memfree(p);
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
			sec_end(mr);
		}

		break;

	case DOC_SEC_SSECT:
	case DOC_SEC_HEAD:
	case DOC_SEC_PARA:
		put_env(mr, flag, ".SH ");
		break;
	case DOC_SEC_SHEAD:
	case DOC_SEC_CAPT:
		put_env(mr, flag, ".SS ");
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

/*	Anf�hrungen
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
	case DOC_MODE_SGML:
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
	case DOC_ENV_TAG:
		if	(flag)
			mroff_cmdline(mr, ".TP");
		mr->item = flag;
		break;
	case DOC_ENV_FIG:
		mroff_cmdline(mr, ".SS Tabelle");
		mr->skip = flag;
		break;
	default:
		return EOF;
	}

	return 0;
}
