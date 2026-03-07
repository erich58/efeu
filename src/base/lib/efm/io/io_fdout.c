#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>
#include <poll.h>
#include <errno.h>

typedef struct {
	StrBuf buf;
	struct pollfd fd;
	int timeout;
} FDOUT;

static void fdout_flush(FDOUT *out);

static int f_put (int c, IO *io);
static int f_ctrl (IO *io, int req, va_list list);

IO *io_fdout(int fd, int timeout, size_t bsize)
{
	FDOUT *fdout = memalloc(sizeof *fdout);
	sb_init(&fdout->buf, bsize);
	sb_expand(&fdout->buf, 1);
	fdout->fd.fd = fd;
	fdout->fd.events = POLLOUT;
	fdout->timeout = timeout;

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

	if	(out->buf.nfree == 0)
		fdout_flush(out);

	c = sb_putc(c, &out->buf);

	if	(c == '\n' || out->buf.nfree == 0)
		fdout_flush(out);

	return c;
}

static void fdout_flush(FDOUT *out)
{
	if	(poll(&out->fd, 1, out->timeout) <= 0)
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

		if	(out->buf.pos)
		{
			errno = EAGAIN;
			return EOF;
		}

		return 0;

	case IO_IDENT:

		*va_arg(list, char **) = msprintf("fdout(%d)", out->fd.fd);
		return 0;

	case IO_FDOUT:

		return out->fd.fd;

	case IO_PENDING:

		return out->buf.pos;

	default:

		return EOF;
	}
}
