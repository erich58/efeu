/*	Ausgabefenster
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <efio.h>

static int win_ctrl(WINDOW *win, int req, va_list list);
static int ow_nctrl(strbuf_t *sb, int req, va_list list);

io_t *OutputWindow(WINDOW *win)
{
	io_t *io;

	io = io_alloc();
	io->get = NULL;
	io->put = (io_put_t) win_put;
	io->data = win;
	io->ctrl = (io_ctrl_t) win_ctrl;
	return io;
}


io_t *io_tmpbuf(size_t size)
{
	io_t *io;

	io = io_alloc();
	io->get = (io_get_t) sb_get;
	io->put = (io_put_t) sb_put;
	io->data = new_strbuf(size);
	io->ctrl = (io_ctrl_t) sb_cctrl;
	return io;
}


static int sb_nctrl(strbuf_t *sb, int req, va_list list)
{
	switch (req)
	{
	case IO_IDENT:	*va_arg(list, char **) = "<strbuf>"; return 0;
	case IO_REWIND:	sb->pos = 0; return 0;
	default:	return EOF;
	}
}


static int sb_cctrl(strbuf_t *sb, int req, va_list list)
{
	switch (req)
	{
	case IO_IDENT:	*va_arg(list, char **) = "<tmpbuf>"; return 0;
	case IO_REWIND:	sb->pos = 0; return 0;
	case IO_CLOSE:	return del_strbuf(sb);
	default:	return EOF;
	}
}
