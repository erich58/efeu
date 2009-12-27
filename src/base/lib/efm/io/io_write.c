/*	Schreiben in Ein/Ausgabestruktur
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>


size_t io_write(io_t *io, const void *buf, size_t nbyte)
{
	if	(io && io->put)
	{
		const uchar_t *ptr = buf;
		size_t n;

		for (n = 0; n < nbyte; ptr++, n++)
			if ((*io->put)(*ptr, io->data) == EOF) break;

		return n;
	}
	else	return 0;
}


size_t io_rwrite(io_t *io, const void *buf, size_t nbyte)
{
	if	(io && io->put)
	{
		const uchar_t *ptr = buf;
		size_t n;

		for (n = 0, ptr += nbyte - 1; n < nbyte; ptr--, n++)
			if ((*io->put)(*ptr, io->data) == EOF) break;

		return n;
	}
	else	return 0;
}
