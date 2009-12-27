/*	Zeichen �berlesen
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>


int io_skip(io_t *io, const char *delim)
{
	int c;

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(listcmp(delim, c))
		{
			io_ungetc(c, io);
			break;
		}
	}

	return c;
}
