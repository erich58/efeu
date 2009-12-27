/*	Umgebungswechsel
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <term.h>

int term_env (term_t *trm, int flag, va_list list)
{
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
			trm->margin -= trm->hangpar ? TERM_INDENT : 0;
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
	case DOC_SEC_CAPT:
	case DOC_SEC_SCAPT:
	case DOC_SEC_MARG:
	case DOC_SEC_NOTE:

		if	(flag)
		{
			term_newline(trm, 1);
			pushstack(&trm->stack, (void *) (size_t) trm->margin);
			term_att(trm, 1, TermPar.bf);
			trm->margin = 0;
		}
		else
		{
			term_att(trm, 1, TermPar.rm);
			trm->margin = (int) (size_t) popstack(&trm->stack, NULL);
			term_newline(trm, 0);
		}

		break;

	case DOC_ENV_MPAGE:

		if	(flag)
		{
			char *num = va_arg(list, char *);
			term_newline(trm, 1);
			term_string(trm, va_arg(list, char *));
			trm->put((DocDrv_t *) trm, '(');
			term_string(trm, num);
			trm->put((DocDrv_t *) trm, ')');
			term_newline(trm, 1);
		}

		break;
/*	Attribute und Anführungen
*/
	case DOC_ATT_RM:	term_att(trm, flag, NULL); break;
	case DOC_ATT_BF:	term_att(trm, flag, TermPar.bf); break;
	case DOC_ATT_IT:	term_att(trm, flag, TermPar.it); break;
	case DOC_ATT_TT:	term_att(trm, flag, TermPar.tt); break;
	case DOC_QUOTE_SGL:	term_putc(trm, '\''); break;
	case DOC_QUOTE_DBL:	term_putc(trm, '"'); break;

/*	Verarbeitungsmodi
*/
	case DOC_MODE_COPY:
		trm->put = flag ? DocDrv_plain : (DocDrvPut_t) term_putc;
		break;
	case DOC_MODE_PLAIN:
		break;
	case DOC_MODE_VERB:
		term_newline(trm, 0);
		term_att(trm, flag, TermPar.tt);
		trm->put = (DocDrvPut_t) (flag ? term_verb : term_putc);
		break;
	case DOC_MODE_SKIP:
	case DOC_MODE_MAN:
	case DOC_MODE_TEX:
	case DOC_MODE_SGML:
		trm->skip = flag;
		break;

/*	Listen
*/
	case DOC_LIST_ITEM:
	case DOC_LIST_ENUM:
	case DOC_LIST_DESC:
		term_newline(trm, 0);
		trm->margin += flag ? TERM_INDENT : -TERM_INDENT;
		break;
		
/*	Sonstige Umgebungen
*/
	case DOC_ENV_INTRO:
		if	(flag)
		{
			term_newline(trm, 1);
			term_att(trm, 1, TermPar.bf);
			term_string(trm, "Vorwort");
			term_att(trm, 0, TermPar.bf);
			term_newline(trm, 1);
		}
		break;
	case DOC_ENV_QUOTE:
		term_newline(trm, 0);
		trm->margin += flag ? TERM_INDENT : -TERM_INDENT;
		break;
	case DOC_ENV_HANG:
		trm->hangpar = flag;
		break;
	case DOC_ENV_CODE:
		term_att(trm, flag, TermPar.tt);
		break;
	case DOC_ENV_FORMULA:
		break;
	case DOC_ENV_TAG:
		if	(flag)
		{
			term_newline(trm, 0);
			term_att(trm, 1, TermPar.bf);
			trm->margin -= TERM_INDENT;
		}
		else
		{
			term_att(trm, 0, TermPar.bf);
			trm->margin += TERM_INDENT;

			if	(trm->col >= trm->margin)
				term_newline(trm, 0);
		}
		break;
	case DOC_ENV_FIG:
		trm->skip = flag;
		break;
	default:
		return EOF;
	}

	return 0;
}
