/*	Lesen von Ein/Ausgabestruktur
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>


size_t io_read(io_t *io, void *buf, size_t nbyte)
{
	uchar_t *p;
	size_t n;
	int c;

	p = (uchar_t *) buf;

	for (n = 0; n < nbyte && (c = io_getc(io)) != EOF; n++)
		p[n] = (uchar_t) c;

	return n;
}


size_t io_rread(io_t *io, void *buf, size_t nbyte)
{
	uchar_t *p;
	size_t n;
	int c;

	p = (uchar_t *) buf;

	for (n = 1; n <= nbyte && (c = io_getc(io)) != EOF; n++)
		p[nbyte - n] = (uchar_t) c;

	return n - 1;
}


/*	Daten in dynamischen Buffer lesen
*/

char *io_mread(io_t *io, size_t size)
{
	char *buf;

	buf = memalloc(size + 1);
	size = io_read(io, buf, size);
	buf[size] = 0;
	return buf;
}
