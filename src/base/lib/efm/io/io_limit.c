#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

typedef struct {
	IO *out;
	size_t size;
} IOLIM;

static int f_put (int c, IO *io);
static int f_ctrl (IO *io, int req, va_list list);

IO *io_limit(IO *out, size_t size)
{
	IOLIM *iolim = memalloc(sizeof *iolim);
	iolim->out = out;
	iolim->size = size;

	IO *io = io_alloc();
	io->put = f_put;
	io->ctrl = f_ctrl;
	io->data = iolim;
	return io;
}

/*	Ausgabe eines Zeichens
*/

static int f_put (int c, IO *io)
{
	IOLIM *iolim = io->data;

	if	(iolim->size)
	{
		--iolim->size;
		return io_putc(c, iolim->out);
	}

	return EOF;
}

/*	Kontrollfunktion
*/

static int f_ctrl (IO *io, int req, va_list list)
{
	IOLIM *lim = io->data;
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		stat = io_close(lim->out);
		memfree(lim);
		return stat;

	default:

		return EOF;
	}
}
