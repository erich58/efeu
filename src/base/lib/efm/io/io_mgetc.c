/*	Filter für Fortsetzungszeilen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/io.h>
#include <EFEU/ioctrl.h>


int io_mgetc (io_t *io, int flag)
{
	register int c, sflag;

	if	(io == NULL || io->stat != IO_STAT_OK)
		return EOF;

	sflag = io->nsave;
	c = io_getc(io);

	while (c == '\\')
	{
		if	(io_peek(io) != '\n')
			return '\\';

		io_getc(io);
		io_linemark(io);

		if	(sflag)
			return '\n';

		c = io_getc(io);

		if	(!flag)
			continue;

		while (c == ' ' || c == '\t')
			c = io_getc(io);
	}

	return c;
}
