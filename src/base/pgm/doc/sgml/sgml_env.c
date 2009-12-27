/*	Umgebungswechsel
	(c) 1999 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

#include <SGML.h>


/*	Hilfskommandos
*/

static void put_att (SGML_t *sgml, int flag, char *att)
{
	if	(sgml->copy)	att = NULL;
	if	(sgml->att)	io_printf(sgml->out, "</%s>", sgml->att);

	if	(flag)
	{
		pushstack(&sgml->s_att, sgml->att);
		sgml->att = att;
	}
	else	sgml->att = popstack(&sgml->s_att, NULL);

	if	(sgml->att)	io_printf(sgml->out, "<%s>", sgml->att);
}

static void put_env (SGML_t *sgml, int flag, const char *name, int nl)
{
	SGML_newline(sgml, 0);
	io_putc('<', sgml->out);

	if	(!flag)
		io_putc('/', sgml->out);

	io_puts(name, sgml->out);
	sgml->last = io_putc('>', sgml->out);

	if	(!flag || nl)
		sgml->last = io_putc('\n', sgml->out);
}

static void put_sect (SGML_t *sgml, int flag, const char *name)
{
	SGML_newline(sgml, 0);

	if	(flag)
	{
		io_putc('\n', sgml->out);
		io_putc('<', sgml->out);
		io_puts(name, sgml->out);
		io_putc('>', sgml->out);
	}
	else	io_puts("<p>\n", sgml->out);
}

static void list_env (SGML_t *sgml, int flag, const char *name)
{
	SGML_newline(sgml, 0);
	put_env(sgml, flag, name, 1);

	if	(!flag)	sgml->nextpar = "\n";
}

int SGML_env (SGML_t *sgml, int flag, va_list list)
{
	int cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_PAR_STD:
		SGML_newline(sgml, 0);

		if	(flag)
		{
			io_puts(sgml->nextpar, sgml->out);
			sgml->nextpar = NULL;
		}
		else	io_putc('\n', sgml->out);

		break;

	case DOC_QUOTE_SGL:
		io_puts(flag ? "`" : "'", sgml->out);
		break;
	case DOC_QUOTE_DBL:
		io_puts(flag ? "&raquo;" : "&laquo;", sgml->out);
		break;

	case DOC_SEC_PART:
	case DOC_SEC_MCHAP:
	case DOC_SEC_CHAP:
		put_sect(sgml, flag, sgml->class ? "chapt" : "sect");
		break;
	case DOC_SEC_SSECT:
		put_sect(sgml, flag, "sect1");
		break;
	case DOC_SEC_HEAD:
	case DOC_SEC_PARA:
	case DOC_SEC_SHEAD:
	case DOC_SEC_MARG:
	case DOC_SEC_NOTE:
		SGML_newline(sgml, 0);
		io_puts(flag ? "\n<bf>" : "</bf><newline>\n", sgml->out);
		break;

	case DOC_ATT_RM:	put_att(sgml, flag, NULL); break;
	case DOC_ATT_BF:	put_att(sgml, flag, "bf"); break;
	case DOC_ATT_IT:	put_att(sgml, flag, "it"); break;
	case DOC_ATT_TT:	put_att(sgml, flag, "tt"); break;

	case DOC_LIST_ITEM:	list_env(sgml, flag, "itemize"); break;
	case DOC_LIST_ENUM:	list_env(sgml, flag, "enum"); break;
	case DOC_LIST_DESC:	list_env(sgml, flag, "descrip"); break;
		
	case DOC_ENV_INTRO:
		SGML_newline(sgml, 0);

		if	(flag)
			io_printf(sgml->out, "<%s>Vorwort\n\n<p>\n",
				sgml->class ? "chapt" : "sect");

		break;
	case DOC_ENV_QUOTE:
		if	(!flag)
		{
			SGML_newline(sgml, 0);
			io_puts("</quote>\n", sgml->out);
		}
		else	sgml->nextpar = "<quote>";

		break;
	case DOC_ENV_MPAGE:
		if	(flag)
		{
			SGML_newline(sgml, 0);
			io_printf(sgml->out, "\n<sect>%s<p>\n",
				va_arg(list, char *));
		}
		else	SGML_newline(sgml, 0);

		break;
	case DOC_MODE_COPY:
		sgml->copy = flag;
		break;
	case DOC_MODE_SKIP:
		sgml->skip = flag;
		break;
	case DOC_MODE_SGML:
	case DOC_MODE_PLAIN:
		sgml->put = flag ? DocDrv_plain : (DocDrvPut_t) SGML_putc;
		break;
	case DOC_MODE_TEX:
	case DOC_MODE_MAN:
		SGML_env(sgml, flag, list);
		break;
	case DOC_MODE_VERB:
		put_env(sgml, flag, "verb", 1);
		sgml->put = (DocDrvPut_t) (flag ? SGML_plain : SGML_putc);
		break;

	case DOC_ENV_FORMULA:
		io_puts(flag ? "<f>" : "</f>", sgml->out); break;
	case DOC_ENV_TAG:
		io_puts(flag ? "<tag>" : "</tag>", sgml->out); break;
	default:
		return EOF;
	}

	return 0;
}
