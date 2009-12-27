/*	Steuerbefehle
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <DocCtrl.h>
#include <SGML.h>
#include <efeudoc.h>


int SGML_cmd (SGML_t *sgml, va_list list)
{
	int cmd;
	
	cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_CMD_TOC:
		SGML_newline(sgml, 0);
		io_puts("<toc>\n", sgml->out);
		break;
	case DOC_CMD_BREAK:
		io_puts("<newline>", sgml->out);
		break;
	case DOC_CMD_NPAGE:
		io_puts("<newpage>", sgml->out);
		break;
	case DOC_CMD_ITEM:
		sgml->nextpar = NULL;
		io_puts("<item>", sgml->out);
		break;
	default:
		return EOF;
	}

	return 0;
}
