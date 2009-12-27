/*	IO-Struktur für HTML-Ausgabe
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

#define	CTRL_NL		0x1
#define	CTRL_CMD	0x2

typedef struct {
	io_t *io;	/* Ausgabestruktur */
	int last;	/* Letztes ausgegebene Zeichen */
	int protect;	/* Schutzmodus */
} FILTER;

static int filter_put (int c, FILTER *filter)
{
	if	(filter->protect)
		return io_putc(c, filter->io);

	switch (c)
	{
	case '<':
		io_puts("&lt;", filter->io);
		break;
	case '>':
		io_puts("&gt;", filter->io);
		break;
	case '&':
		io_puts("&amp;", filter->io);
		break;
	case '"':
		io_puts("&quot;", filter->io);
		break;
	default:
		io_putc(c, filter->io);
		break;
	}

	filter->last = c;
	return c;
}

static int filter_ctrl(FILTER *filter, int req, va_list list)
{
	char *fmt;

	switch (req)
	{
	case IO_CLOSE:
		io_puts("</HTML></BODY>\n", filter->io);
		io_close(filter->io);
		memfree(filter);
		return io_close_stat;
	case CTRL_NL:
		if	(filter->last != '\n')
		{
			io_putc('\n', filter->io);
			filter->last = '\n';
		}
	case IO_PROTECT:

		if	(va_arg(list, int))	filter->protect++;
		else if	(filter->protect)	filter->protect--;

		return filter->protect;
	case CTRL_CMD:
		fmt = va_arg(list, char *);
		io_vprintf(filter->io, fmt, list);
		return 0;
	default:
		break;
	}

	return io_vctrl(filter->io, req, list);
}


io_t *io_html (io_t *io)
{
	if	(io)
	{
		FILTER *filter;

		filter = memalloc(sizeof(FILTER));
		filter->io = io;
		filter->last = '\n';
		filter->protect = 0;
		io_puts("<HTML><BODY>\n", io);
		io = io_alloc();
		io->put = (io_put_t) filter_put;
		io->ctrl = (io_ctrl_t) filter_ctrl;
		io->data = filter;
	}

	return io;
}
