/*	Ausgabekontrolle für sc
	(c) 1994 Erich Frühstück
*/

#include <EFEU/pctrl.h>

static int max_strlen = 0;
static int max_label = 0;
static int max_width = 0;
static int max_prec = 0;
static int max_col = 0;

static int count_idx = 0;
static int count_buf[2] = { 0, 0 };
static int count_len = 0;

static char *label = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static int put_xpos (io_t *io, int pos);
static int val2_put (int c, PrFilter_t *pf);

#define newline(pf)	io_puts(pf->nl, pf->io)


#define	SC_HEAD	"#       Aufbereitete Datenmatrix fuer Spreadsheet Calculator."

int sc_ctrl(PrFilter_t *pf, int cmd, va_list list)
{
	char *a;
	int i;

	switch (cmd)
	{
	case PCTRL_BEGIN:

		io_puts(SC_HEAD, pf->io);
		newline(pf);
		newline(pf);
		pf->delim = "\n";
		max_strlen = 0;
		max_label = 0;
		max_width = 0;
		max_prec = 0;
		max_col = 0;
		count_idx = 0;
		count_buf[0] = count_buf[1] = 0;
		return 0;

	case PCTRL_DATA:

		max_strlen = 0;
		max_label = 0;
		max_width = 0;
		max_prec = 0;
		max_col = 0;
		return 0;

	case PCTRL_END:

		io_printf(pf->io, "format A %d 0 0\n", max_strlen);

		if	(max_prec != 0)
			max_width += max_prec;

		if	(max_width < max_label)
			max_width = max_label;

		max_width++;

		for (i = 1; i <= max_col; i++)
		{
			io_puts("format ", pf->io);
			put_xpos(pf->io, i);
			io_printf(pf->io, " %d %d 0\n",
				max_width, max_prec);
		}

		io_psub(pf->io, "goto A0\n");
		return 0;

	case PCTRL_LEFT:	a = "leftstring "; break;
	case PCTRL_CENTER:	a = "label "; break;
	case PCTRL_RIGHT:	a = "rightstring "; break;
	case PCTRL_EMPTY:	return 0;
	case PCTRL_VALUE:	a = "let "; break;
	default:		return EOF;
	}

	io_puts(a, pf->io);
	put_xpos(pf->io, pf->col - 1);
	io_printf(pf->io, "%d", pf->line - 1);
	io_puts(" = ", pf->io);

	switch (cmd)
	{
	case PCTRL_LEFT:
	case PCTRL_CENTER:
	case PCTRL_RIGHT:

		count_len = 0;
		io_putc('"', pf->io);
		pf->put = sc_put;
		break;

	case PCTRL_EMPTY:

		io_puts("0", pf->io);
		break;

	case PCTRL_VALUE:

		count_buf[0] = count_buf[1] = 0;
		count_idx = 0;
		pf->put = val2_put;
		break;
	}

	return 0;
}


static int put_xpos(io_t *io, int pos)
{
	char buf[3];

	if	(max_col < pos)
		max_col = pos;

	buf[2] = 0;
	buf[1] = label[pos % 26];

	if	(pos < 26)
		return io_puts(buf + 1, io);

	pos = (pos / 26) - 1;
	buf[0] = label[pos % 26];
	return io_puts(buf, io);
}


int sc_put(int c, PrFilter_t *pf)
{
	switch (c)
	{
	case EOF:

		if	(pf->col <= 1 && count_len > max_strlen)
		{
			max_strlen = count_len;
		}
		else if	(count_len > max_label)
		{
			max_label = count_len;
		}

		c = '"';
		break;

	case '"':
	
		count_len += 2;
		return io_puts("\\\"", pf->io);
		break;

	default:

		count_len++;
		break;
	}

	return io_putc(c, pf->io);
}


static int val2_put(int c, PrFilter_t *pf)
{
	switch (c)
	{
	case EOF:

		if	(max_width < count_buf[0])
			max_width = count_buf[0];

		if	(max_prec < count_buf[1])
			max_prec = count_buf[1];

		return 0;

	case '\t':
	case '\n':
	
		c = ' ';
		break;

	case '.':
	
		count_buf[count_idx]++;
		count_idx = 1;
		return io_putc(c, pf->io);

	default:
	
		break;
	}

	count_buf[count_idx]++;
	return io_putc(c, pf->io);
}
