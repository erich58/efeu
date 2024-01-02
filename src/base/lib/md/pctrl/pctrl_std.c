/*	Ausgabekontrolle für Standardanwendung
	(c) 1994 Erich Frühstück
*/

#include <EFEU/pctrl.h>

static int std_ctrl(PCTRL *pf, int cmd, va_list list);
static int data_ctrl (PCTRL *pf, int cmd, va_list list);
static int std_put(int c, PCTRL *pf);

PCTRLDef pctrl_std = {
	NULL, "Standard output (space delimiter)", " ", "\n", "\f", std_ctrl
};

PCTRLDef pctrl_ascii = {
	"ascii", "Ascii output (tab delimiter)", "\t", "\n", "\f", std_ctrl
};

PCTRLDef pctrl_data = {
	"data", "Data without labels", NULL, "\n", NULL, data_ctrl
};

static int std_ctrl(PCTRL *pf, int cmd, va_list list)
{
	if	(cmd & PCTRL_FMASK)	pf->put = std_put;

	return 0;
}

static int std_put(int c, PCTRL *pf)
{
	switch (c)
	{
	case EOF:	return 0;
	case '\t':
	case '\n':	c = ' '; break;
	default:	break;
	}

	return io_putc(c, pf->io);
}

static int data_ctrl(PCTRL *pf, int cmd, va_list list)
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
