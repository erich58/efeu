/*	Ausgabekontrolle für Standardanwendung
	(c) 1994 Erich Frühstück
*/

#include <EFEU/pctrl.h>


int std_ctrl(PCTRL *pf, int cmd, va_list list)
{
	if	(cmd & PCTRL_FMASK)	pf->put = std_put;

	return 0;
}


int std_put(int c, PCTRL *pf)
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


int tst_ctrl(PCTRL *pf, int cmd, va_list list)
{
	char *a;
	int flag;

	a = NULL;
	flag = 0;

	switch (cmd)
	{
	case PCTRL_BEGIN:	a = "BEGIN\n"; break;
	case PCTRL_DATA:	a = "DATA\n"; break;
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


int tst_put(int c, PCTRL *pf)
{
	return (c == EOF) ? io_puts(")", pf->io) : io_mputc(c, pf->io, "()");
}
