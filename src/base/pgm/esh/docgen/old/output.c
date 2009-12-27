/*	Ausgabefilter
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include "eisdoc.h"
#include <EFEU/ioctrl.h>


typedef struct {
	io_t *out;		/* Ausgabestruktur */
	Filter_t *filter;	/* Ausgabefilter */
	int pos;		/* Aktuelle Position mit Filter */
	int outchar;		/* Ausgabezeile angefangen */
	int mode;		/* Ausgabemodus */
} OFILTER;

static int ofilter_put(int c, OFILTER *ofilter)
{
	if	(c == '\n')
	{
		io_putc(c, ofilter->out);
		ofilter->outchar = 0;
		ofilter->pos = 0;
	}
	else if	(c == '\t' && (ofilter->mode & OMODE_EXPAND))
	{
		do	ofilter_put(' ', ofilter);
		while	(ofilter->pos % 8 != 0);
	}
	else if	(ofilter->mode & OMODE_FILTER)
	{
		if	(ofilter->filter)
			ofilter->filter->put(c, ofilter->out);
		else	io_putc(c, ofilter->out);

		ofilter->pos++;
		ofilter->outchar = 1;
	}
	else
	{
		io_putc(c, ofilter->out);
		ofilter->outchar = 1;
	}

	return 1;
}

static int ofilter_ctrl(OFILTER *ofilter, int req, va_list list)
{
	size_t stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(ofilter->out);
		memfree(ofilter);
		break;

	case IO_REWIND:

		stat = io_rewind(ofilter->out);
		ofilter->pos = 0;
		break;

	case OUT_NEWLINE:

		if	(ofilter->pos)
			io_putc('\n', ofilter->out);

		ofilter->pos = 0;
		break;

	case OUT_MODE:

		stat = ofilter->mode;
		ofilter->mode = va_arg(list, int);
		break;

	case OUT_FILTER:

		ofilter->filter = GetFilter(va_arg(list, char *));
		stat = ofilter->filter ? 1 : 0;
		break;

	default:

		stat = io_vctrl(ofilter->out, req, list);
		break;
	}

	return stat;
}

io_t *OutputFilter (io_t *io)
{
	if	(io)
	{
		OFILTER *ofilter = memalloc(sizeof(OFILTER));
		ofilter->out = io;
		ofilter->pos = 0;
		ofilter->mode = 0;
		ofilter->filter = NULL;
		io = io_alloc();
		io->put = (io_put_t) ofilter_put;
		io->ctrl = (io_ctrl_t) ofilter_ctrl;
		io->data = ofilter;
	}

	return io;
}

int set_mode(io_t *out, int mode)
{
	return io_ctrl(out, OUT_MODE, mode);
}
