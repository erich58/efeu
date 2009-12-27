/*	Steuerbefehle
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <DocCtrl.h>
#include <mroff.h>
#include <efeudoc.h>

int mroff_cmd (mroff_t *mr, va_list list)
{
	int cmd;
	
	cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_CMD_TOC:
		mroff_newline(mr);
		mroff_rem(mr, "Inhaltsverzeichnis");
		break;
	case DOC_CMD_BREAK:
		mroff_cmdline(mr, mr->item ? ".TP" : ".br");
		break;
	case DOC_CMD_ITEM:
		mroff_cmdline(mr, ".IP *");
		break;
	default:
		return EOF;
	}

	return 0;
}
