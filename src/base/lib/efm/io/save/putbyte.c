/*	Schreiben in Ein/Ausgabestruktur
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efio.h>


/*	Zahlenwerte ausgeben
*/

int io_putshort(int val, io_t *io)
{
	short sval = val;
	return io_write(io, &sval, sizeof(short));
}


int io_putlong(long int val, io_t *io)
{
	return io_write(io, &val, sizeof(long));
}


/*	Maschinenunabhängige Ausgabe der letzten Byte eines long - Wertes
*/

int io_putbyte(unsigned val, io_t *io, int byte)
{
	uchar_t *data;
	int n;

	data = (uchar_t *) &val;
	n = 0;

	switch (byte)
	{
	default:	n += io_nputc(0, io, byte - 4);
#if	REVBYTEORDER
	case  4:	io_putc(data[3], io);
	case  3:	io_putc(data[2], io);
	case  2:	io_putc(data[1], io);
	case  1:	io_putc(data[0], io);
#else
	case  4:	io_putc(data[0], io);
	case  3:	io_putc(data[1], io);
	case  2:	io_putc(data[2], io);
	case  1:	io_putc(data[3], io);
#endif
	case  0:	break;
	}

	return byte;
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
		n = 1;
		n += io_putbyte(size, io, byte);
		n += io_write(io, str, size);
	}
	else	n = 0;

	return;
}
