#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <poll.h>

typedef struct {
	StrBuf buf;
	struct pollfd fd;
} FDOUT;

static void fdout_flush(FDOUT *out);

static int f_put (int c, IO *io);
static int f_ctrl (IO *io, int req, va_list list);

IO *io_fdout(int fd, size_t bsize)
{
	FDOUT *fdout = memalloc(sizeof *fdout);
	sb_init(&fdout->buf, bsize);
	fdout->fd.fd = fd;
	fdout->fd.events = POLLOUT;

	IO *io = io_alloc();
	io->put = f_put;
	io->ctrl = f_ctrl;
	io->data = fdout;
	return io;
}

/*	Ausgabe eines Zeichens
*/

static int f_put (int c, IO *io)
{
	FDOUT *out = io->data;
	c = sb_putc(c, &out->buf);

	if	(c == '\n' || out->buf.nfree == 0)
		fdout_flush(out);

	return c;
}

static void fdout_flush(FDOUT *out)
{
	if	(poll(&out->fd, 1, 0) <= 0)
		return;

	ssize_t s = write(out->fd.fd, out->buf.data, out->buf.pos);

	if	(s <= 0)
	{
		;
	}
	else if	(s < out->buf.pos)
	{
		out->buf.pos -= s;
		memcpy(out->buf.data, out->buf.data + s, out->buf.pos);
		sb_sync(&out->buf);
	}
	else sb_trunc(&out->buf);
}


/*	Kontrollfunktion
*/

static int f_ctrl (IO *io, int req, va_list list)
{
	FDOUT *out = io->data;
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = close(out->fd.fd);
		sb_free(&out->buf);
		memfree(out);
		return stat;

	case IO_FLUSH:

		if	(out->buf.pos)
			fdout_flush(out);

		return out->buf.pos;

	case IO_IDENT:

		*va_arg(list, char **) = msprintf("fdout(%d)", out->fd.fd);
		return 0;

	case IO_GETPOS:
		*va_arg(list, unsigned *) = out->buf.pos;
		return 0;

	case IO_FDOUT:

		return out->fd.fd;

	default:

		return EOF;
	}
}
