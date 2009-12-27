/*	Lesen von Ein/Ausgabestruktur
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>


/*	Zahlenwerte einlesen
*/

short io_getshort(io_t *io)
{
	short val;

	if 	(io_read(io, &val, sizeof(short)) != sizeof(short))
	{
		return 0;
	}
	else	return val;
}


long io_getlong(io_t *io)
{
	long val;

	if 	(io_read(io, &val, sizeof(long)) != sizeof(long))
	{
		return 0;
	}
	else	return val;
}


/*	Maschinenunabhängige Eingabe der letzten Byte eines unsigned - Wertes
*/

unsigned io_getbyte(io_t *io, int byte)
{
	int c;
	unsigned val;
	uchar_t *data;

	val = 0;
	data = (uchar_t *) &val;

	switch (byte)
	{
	default:	while (byte-- > 4) io_getc(io);
#if	REVBYTEORDER
	case  4:	if ((c = io_getc(io)) != EOF) data[3] = c;
	case  3:	if ((c = io_getc(io)) != EOF) data[2] = c;
	case  2:	if ((c = io_getc(io)) != EOF) data[1] = c;
	case  1:	if ((c = io_getc(io)) != EOF) data[0] = c;
#else
	case  4:	if ((c = io_getc(io)) != EOF) data[0] = c;
	case  3:	if ((c = io_getc(io)) != EOF) data[1] = c;
	case  2:	if ((c = io_getc(io)) != EOF) data[2] = c;
	case  1:	if ((c = io_getc(io)) != EOF) data[3] = c;
#endif
	case  0:	break;
	}

	return val;
}


/*	Gepackten String einlesen
*/

char *io_getstr(io_t *io)
{
	int byte;
	ulong_t size;

	byte = io_getc(io);

	if	(byte <= 0)	return NULL;

	size = io_getbyte(io, byte);
	return io_mread(io, size);
}
