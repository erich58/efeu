#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/mstring.h>

typedef struct {
	StrBuf buf;
	IO *out;
	char *delim;
} HEREDOC;

static int f_put (int c, IO *io);
static int f_ctrl (IO *io, int req, va_list list);

IO *io_heredoc(IO *out, const char *delim)
{
	HEREDOC *hdoc = memalloc(sizeof *hdoc);
	sb_init(&hdoc->buf, 512);
	hdoc->out = out;
	hdoc->delim = mstrcpy(delim ? delim : "");

	IO *io = io_alloc();
	io->put = f_put;
	io->ctrl = f_ctrl;
	io->data = hdoc;
	return io;
}

/*	Ausgabe eines Zeichens
*/

static int f_put (int c, IO *io)
{
	HEREDOC *hdoc = io->data;

	if	(!hdoc->out)
		return EOF;

	if	(c == '\n')
	{
		const char *p = sb_cstr(&hdoc->buf);

		if	(strcmp(p, hdoc->delim) != 0)
		{
			io_puts(p, hdoc->out);
			io_putc(c, hdoc->out);
			sb_trunc(&hdoc->buf);
		}
		else
		{
			io_close(hdoc->out);
			hdoc->out = NULL;
		}
	}
	else
	{
		c = sb_putc(c, &hdoc->buf);
	}

	return c;
}

/*	Kontrollfunktion
*/

static int f_ctrl (IO *io, int req, va_list list)
{
	HEREDOC *hdoc = io->data;
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(hdoc->out);
		sb_free(&hdoc->buf);
		memfree(hdoc->delim);
		memfree(hdoc);
		return stat;

	default:

		return EOF;
	}
}
