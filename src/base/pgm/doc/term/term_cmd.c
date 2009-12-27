/*	Steuerbefehle
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <DocCtrl.h>
#include <term.h>
#include <efeudoc.h>

int term_cmd (term_t *trm, va_list list)
{
	int cmd;
	
	cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_CMD_TOC:
		term_newline(trm, 1);
		term_att(trm, 1, TermPar.bf);
		term_string(trm, "Inhalt");
		term_att(trm, 1, TermPar.bf);
		term_newline(trm, 1);
		break;
	case DOC_CMD_BREAK:
		term_newline(trm, 0);
		break;
	case DOC_CMD_ITEM:
		term_newline(trm, 0);
		trm->margin -= TERM_INDENT;
		term_putc(trm, '*');
		trm->margin += TERM_INDENT;
		break;
	default:
		return EOF;
	}

	return 0;
}
