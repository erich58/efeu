/*	Binäre Ein- Ausgabe von Ganzzahlwerten
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/


#include <EFEU/io.h>


unsigned io_getval (io_t *io, int n)
{
	unsigned val;
	int c;

	for (val = 0; n > 0; n--)
	{
		if	((c = io_getc(io)) == EOF)
		{
			fprintf(stderr, "getval: Unerwartetes Fileende.\n");
			exit(EXIT_FAILURE);
		}

		val = (val << 8) + c;
	}

	return val;
}

void io_putval (unsigned val, io_t *io, int n)
{
	while (n-- > 0)
	{
		if	(io_putc((val >> (8 * n)) & 0xFF, io) == EOF)
		{
			fprintf(stderr, "putval: Ausgabefehler.\n");
			exit(EXIT_FAILURE);
		}
	}
}

/*	Gepackten String einlesen
*/

char *io_getstr(io_t *io)
{
	int byte;
	ulong_t size;

	byte = io_getc(io);

	if	(byte <= 0)	return NULL;

	size = io_getval(io, byte);
	return io_mread(io, size);
}

/*	Gepackte Ausgabe eines Strings mit Längenangabe
*/

int io_putstr(const char *str, io_t *io)
{
	ulong_t size;
	int byte;
	int n;

	if	(str == NULL)	return io_nputc(0, io, 1);

	size = strlen(str);

	if	(size & 0xFF000000)	byte = 4;
	else if	(size & 0x00FF0000)	byte = 3;
	else if	(size & 0x0000FF00)	byte = 2;
	else				byte = 1;

	if	(io_putc(byte, io) != EOF)
	{
		n = 1 + byte;
		io_putval(size, io, byte);
		n += io_write(io, str, size);
	}
	else	n = 0;

	return n;
}
