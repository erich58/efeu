/*	Ausgabekontrolle für Standardanwendung
	(c) 1994 Erich Frühstück
*/

#include <EFEU/pctrl.h>

static int struct_ctrl (PCTRL *pf, int cmd, va_list list);
static int csv_ctrl (PCTRL *pf, int cmd, va_list list);

PCTRLDef pctrl_struct = {
	"struct", "List formatting", ", ", " },\n", NULL, struct_ctrl
};

PCTRLDef pctrl_csv = {
	"csv", "csv-file", ";", "\r\n", NULL, csv_ctrl
};

static int val_put (int c, PCTRL *pf);
static int csv_put (int c, PCTRL *pf);
static int str_put (int c, PCTRL *pf);

static int struct_ctrl(PCTRL *pf, int cmd, va_list list)
{
	switch (cmd)
	{
	case PCTRL_BEGIN:	pf->delim = ", "; break;
	case PCTRL_LEFT:
	case PCTRL_CENTER:
	case PCTRL_RIGHT:	io_puts("\t{ \"", pf->io); pf->put = str_put; break;
	case PCTRL_EMPTY:
	case PCTRL_VALUE:	pf->put = val_put; break;
	default:		break;
	}

	return 0;
}

static int csv_ctrl(PCTRL *pf, int cmd, va_list list)
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

static int csv_put(int c, PCTRL *pf)
{
	switch (c)
	{
	case EOF:	c = '"'; break;
	case '"':	return io_puts("\"\"", pf->io); break;
	default:	break;
	}

	return io_putc(c, pf->io);
}

static int str_put(int c, PCTRL *pf)
{
	switch (c)
	{
	case EOF:	c = '"'; break;
	case '"':	return io_puts("\\\"", pf->io); break;
	default:	break;
	}

	return io_putc(c, pf->io);
}

static int val_put(int c, PCTRL *pf)
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
