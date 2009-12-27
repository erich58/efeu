/*	Ausgabefilter für Informationsdaten
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

typedef struct {
	io_t *io;	/* Ausgabestruktur */
	int last;	/* Letztes ausgegebene Zeichen */
	InfoNode_t *base;	/* Basisknoten */
} FILTER;

static int filter_put(int c, FILTER *filter)
{
	if	(c == INFO_KEY && filter->last == '\n')
		io_putc('\\', filter->io);

	io_putc(c, filter->io);
	filter->last = c;
	return c;
}

static int filter_ctrl(FILTER *filter, int req, va_list list)
{
	int stat;
	char *name, *label;

	switch (req)
	{
	case IO_CLOSE:

		if	(filter->last != '\n')
			filter_put('\n', filter);			

		stat = io_close(filter->io);
		memfree(filter);
		return stat;

	case INFOCTRL_NL:

		if	(filter->last != '\n')
			filter_put('\n', filter);			

		return 0;

	case INFOCTRL_KEY:

		if	(filter->last != '\n')
			filter_put('\n', filter);			

		name = va_arg(list, char *);
		label = name ? va_arg(list, char *) : NULL;

		if	(name || filter->base->name)
		{
			io_putc(INFO_KEY, filter->io);
			filter->last = INFO_KEY;

			InfoName(filter->io, NULL, filter->base);

			if	(name)
			{
				io_putc(INFO_SEP, filter->io);
				io_puts(name, filter->io);
			}

			if	(label)
			{
				io_putc('\t', filter->io);
				io_puts(label, filter->io);
			}

			io_putc('\n', filter->io);
		}

		return 0;

	default:

		break;
	}

	return io_vctrl(filter->io, req, list);
}

io_t *InfoFilter (io_t *io, InfoNode_t *base)
{
	if	(io && base)
	{
		FILTER *filter = memalloc(sizeof(FILTER));

		filter->io = io;
		filter->last = '\n';
		filter->base = base;
		io = io_alloc();
		io->put = (io_put_t) filter_put;
		io->ctrl = (io_ctrl_t) filter_ctrl;
		io->data = filter;
	}

	return io;
}
