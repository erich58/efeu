/*	Filter für CR-LF
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/ioctrl.h>


static int cr_get (io_t *io);
static int cr_ctrl (io_t *io, int req, va_list list);

io_t *io_crfilter(io_t *io)
{
	io_t *new;

	new = io_alloc();
	new->get = (io_get_t) cr_get;
	new->ctrl = (io_ctrl_t) cr_ctrl;
	new->data = io;
	return new;
}


/*	Zeichen lesen
*/

static int cr_get(io_t *io)
{
	int c;

	do	c = io_getc(io);
	while	(c == '\r' || c == 32);

	return c;
}


/*	Kontrollfunktion
*/

static int cr_ctrl(io_t *io, int req, va_list list)
{
	switch (req)
	{
	case IO_CLOSE:	return io_close(io);
	case IO_REWIND:	return io_rewind(io);
	default:	return io_vctrl(io, req, list);
	}
}
