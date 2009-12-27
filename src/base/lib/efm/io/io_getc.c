/*	Zeichen lesen/zurückschreiben
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

/*	Zeichen lesen
*/

int io_getc (io_t *io)
{
	if	(io == NULL || io->stat != IO_STAT_OK)
		return EOF;

	if	(io->nsave)
		return io->save_buf[--io->nsave];

	if	(io->get)
	{
		register int c;

		if	((c = (*io->get)(io->data)) != EOF)
			return c;

		if	(io_ctrl(io, IO_RESTORE, io, NULL) != EOF)
			return io_getc(io);
		
		io->stat |= IO_STAT_EOF;
	}

	return EOF;
}

/*	Zeichen testen
*/

int io_peek (io_t *io)
{
	register int c;

	if	(io == NULL || io->stat != IO_STAT_OK)
		return EOF;

	if	(io->nsave != 0)
		return io->save_buf[io->nsave - 1];
	
	if	((c = io_ctrl(io, IO_PEEK)) != EOF)
		return c;

	if	(io->get)
	{
		if	((c = (*io->get)(io->data)) != EOF)
			return io->save_buf[io->nsave++] = (uchar_t) c;

		if	(io_ctrl(io, IO_RESTORE, io, NULL) != EOF)
			return io_peek(io);

		io->stat |= IO_STAT_EOF;
	}

	return EOF;
}

/*	Zeichen zurückschreiben
*/

int io_ungetc (int c, io_t *io)
{
	if	(io == NULL || c == EOF)
		return 0;

	if	(io->nsave >= IO_MAX_SAVE)
	{
		register int i;

		if	(io_ctrl(io, IO_UNGETC, io->save_buf[0]) == EOF)
			return 0;

		if	(io->stat == IO_STAT_EOF)
			io->stat = IO_STAT_OK;

		io->nsave--;

		for (i = 0; i < io->nsave; i++)
			io->save_buf[i] = io->save_buf[i + 1];
	}

	io->save_buf[io->nsave++] = (uchar_t) c;
	return 1;
}
