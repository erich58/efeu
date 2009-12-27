/*	Ausgabekontrolle für Standardanwendung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pctrl.h>

int data_ctrl(PrFilter_t *pf, int cmd, va_list list)
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
