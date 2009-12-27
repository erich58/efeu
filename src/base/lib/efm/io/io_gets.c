/*	String einlesen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>


char *io_gets (char *buf, int n, io_t *io)
{
	int c;
	int k;

	k = 0;
	n--;

	while (k < n && (c = io_getc(io)) != EOF)
	{
		buf[k++] = (char) c;

		if	(c == '\n')	break;
	}

	if	(k > 0)
	{
		buf[k] = 0;
		return buf;
	}
	else	return NULL;
}
