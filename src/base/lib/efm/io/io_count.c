/*	Zahl der ausgegebenen Zeichen zählen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>


typedef struct {
	io_t *io;	/* Ausgabestruktur */
	size_t count;	/* Zahl der ausgegebenen Zeichen */
} COUNT;

static int std_put(int c, COUNT *count)
{
	if	((c = io_putc(c, count->io)) != EOF)
		count->count++;

	return c;
}

static int dummy_put(int c, COUNT *count)
{
	count->count++;
	return c;
}

static int count_ctrl(COUNT *count, int req, va_list list)
{
	size_t stat;

	switch (req)
	{
	case IO_CLOSE:

		io_close(count->io);
		stat = count->count;
		memfree(count);
		break;

	case IO_REWIND:

		if	(io_rewind(count->io) == EOF)
			return EOF;

		stat = count->count;
		count->count = 0;
		break;

	default:

		stat = io_vctrl(count->io, req, list);
		break;
	}

	return stat;
}


io_t *io_count (io_t *io)
{
	COUNT *count = memalloc(sizeof(COUNT));
	count->io = io;
	count->count = 0;
	io = io_alloc();
	io->put = (io_put_t) (io ? std_put : dummy_put);
	io->ctrl = (io_ctrl_t) count_ctrl;
	io->data = count;
	return io;
}
