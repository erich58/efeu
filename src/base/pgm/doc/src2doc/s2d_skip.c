/*	Lesehilfsfunktionen
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include "src2doc.h"
#include <ctype.h>

int skip_blank (io_t *ein)
{
	int c;

	while ((c = io_skipcom(ein, NULL, 0)) != EOF)
		if (c != ' ' && c != '\t') return c;

	return EOF;
}

int skip_space (io_t *ein, strbuf_t *buf)
{
	int c;

	while ((c = io_skipcom(ein, buf, 1)) != EOF)
		if (!isspace(c)) return c;

	return EOF;
}


int test_key (io_t *io, const char *key)
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

	if	(key[i] == 0)	return 1;

	while (i-- > 0)
		io_ungetc(key[i], io);

	return 0;
}
