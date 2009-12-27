/*	Ausgabekontrolle für Standardanwendung
	(c) 1994 Erich Frühstück
*/

#include <EFEU/pctrl.h>

int data_ctrl(PCTRL *pf, int cmd, va_list list)
{
	switch (cmd)
	{
	case PCTRL_VALUE:
	
		pf->nl = "\n";
		pf->delim = PrintListDelim;
		pf->put = std_put;
		break;

	default:
	
		pf->nl = NULL;
		pf->delim = NULL;
		break;
	}

	return 0;
}
