/*	Ausgabekontrolle für Standardanwendung
	(c) 2024 Erich Frühstück
*/

#include <EFEU/pctrl.h>

static int tst_put(int c, PCTRL *pf);
static int tst_ctrl(PCTRL *pf, int cmd, va_list list);

PCTRLDef pctrl_test = {
	"test", "Test output", ", ", "<NL>\n", "<FF>\f", tst_ctrl
};

static int tst_ctrl(PCTRL *pf, int cmd, va_list list)
{
	char *a;
	int flag;

	a = NULL;
	flag = 0;

	switch (cmd)
	{
	case PCTRL_BEGIN:	a = "BEGIN\n"; break;
	case PCTRL_DATA:
		io_xprintf(pf->io, "DATA(%d)\n", va_arg(list, int));
		return 0;
	case PCTRL_HEAD:	a = "HEAD\n"; break;
	case PCTRL_LINE:	break;
	case PCTRL_PAGE:	break;
	case PCTRL_EHEAD:	a = "EHEAD\n"; break;
	case PCTRL_EDATA:	a = "EDATA\n"; break;
	case PCTRL_END:		a = "END\n"; break;
	case PCTRL_LEFT:	flag = 1; a = "LEFT("; pf->put = tst_put; break;
	case PCTRL_CENTER:	flag = 1; a = "CENTER("; pf->put = tst_put; break;
	case PCTRL_RIGHT:	flag = 1; a = "RIGHT("; pf->put = tst_put; break;
	case PCTRL_EMPTY:	flag = 1; a = "EMPTY("; pf->put = tst_put; break;
	case PCTRL_VALUE:	flag = 1; a = "VAL("; pf->put = tst_put; break;
	default:		io_xprintf(pf->io, "<%X>\n", cmd); return EOF;
	}

	if	(flag)	io_xprintf(pf->io, "<%d,%d>", pf->line, pf->col);

	io_puts(a, pf->io);
	return 0;
}


static int tst_put(int c, PCTRL *pf)
{
	return (c == EOF) ? io_puts(")", pf->io) : io_mputc(c, pf->io, "()");
}
