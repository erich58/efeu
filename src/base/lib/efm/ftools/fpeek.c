/*	Nächstes Zeichen eines Datenfiles abfragen
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>
#include <EFEU/ftools.h>

int fpeek (FILE *file)
{
	register int c = getc(file);

	if	(c != EOF)
		ungetc(c, file);

	return c;
}
