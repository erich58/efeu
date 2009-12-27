/*	Schlüsselword testen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <ctype.h>

#define	ENDNAME(c)	(c != '_' && !isalnum(c))


int io_testkey(io_t *io, const char *key)
{
	int i;
	int c;

	if	(key == NULL)	return 0;

	for (i = 0; ; i++)
	{
		c = io_getc(io);

		if	(key[i] == 0 || c != key[i])	break;
	}

	io_ungetc(c, io);

	if	(key[i] == 0 && ENDNAME(c))	return 1;

	while (i-- > 0)
		io_ungetc(key[i], io);

	return 0;
}
