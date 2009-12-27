/*	Ausgabekontrolle für Standardanwendung
	(c) 1994 Erich Frühstück
*/

#include <EFEU/pctrl.h>


int struct_ctrl(PrFilter_t *pf, int cmd, va_list list)
{
	switch (cmd)
	{
	case PCTRL_BEGIN:	pf->delim = ", "; break;
	case PCTRL_LEFT:
	case PCTRL_CENTER:
	case PCTRL_RIGHT:	io_puts("\t{ \"", pf->io); pf->put = str_put; break;
	case PCTRL_EMPTY:
	case PCTRL_VALUE:	pf->put = std_put; break;
	default:		break;
	}

	return 0;
}


int csv_ctrl(PrFilter_t *pf, int cmd, va_list list)
{
	switch (cmd)
	{
	case PCTRL_BEGIN:	pf->delim = ";"; break;
	case PCTRL_LEFT:
	case PCTRL_CENTER:
	case PCTRL_RIGHT:	io_putc('"', pf->io); pf->put = csv_put; break;
	case PCTRL_EMPTY:
	case PCTRL_VALUE:	pf->put = val_put; break;
	default:		break;
	}

	return 0;
}


int csv_put(int c, PrFilter_t *pf)
{
	switch (c)
	{
	case EOF:	c = '"'; break;
	case '"':	return io_puts("\"\"", pf->io); break;
	default:	break;
	}

	return io_putc(c, pf->io);
}


int str_put(int c, PrFilter_t *pf)
{
	switch (c)
	{
	case EOF:	c = '"'; break;
	case '"':	return io_puts("\\\"", pf->io); break;
	default:	break;
	}

	return io_putc(c, pf->io);
}


int val_put(int c, PrFilter_t *pf)
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
