/*	IO Schnittstelle zu dynamischen Stringbuffer
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/strbuf.h>

static int sb_cctrl(strbuf_t *sb, int req, va_list list);
static int sb_nctrl(strbuf_t *sb, int req, va_list list);

io_t *io_tmpbuf (size_t size)
{
	io_t *io;

	io = io_alloc();
	io->get = (io_get_t) sb_get;
	io->put = (io_put_t) sb_put;
	io->data = new_strbuf(size);
	io->ctrl = (io_ctrl_t) sb_cctrl;
	return io;
}

io_t *io_strbuf (strbuf_t *buf)
{
	io_t *io;

	io = io_alloc();
	io->get = (io_get_t) sb_get;
	io->put = (io_put_t) sb_put;

	if	(buf == NULL)
	{
		io->data = new_strbuf(0);
		io->ctrl = (io_ctrl_t) sb_cctrl;
	}
	else
	{
		io->data = buf;
		io->ctrl = (io_ctrl_t) sb_nctrl;
	}

	return io;
}



static int sb_nctrl (strbuf_t *sb, int req, va_list list)
{
	switch (req)
	{
	case IO_REWIND:	sb->pos = 0; return 0;
	case IO_CLOSE:	return 0;
	case IO_IDENT:	*va_arg(list, char **) = "<strbuf>"; return 0;
	default:	return EOF;
	}
}


static int sb_cctrl (strbuf_t *sb, int req, va_list list)
{
	switch (req)
	{
	case IO_REWIND:	sb->pos = 0; return 0;
	case IO_CLOSE:	return del_strbuf(sb);
	case IO_IDENT:	*va_arg(list, char **) = "<tmpbuf>"; return 0;
	default:	return EOF;
	}
}
