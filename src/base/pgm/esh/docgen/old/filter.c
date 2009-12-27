/*	Ausgabefilter
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"
#include <ctype.h>

static void plain_putc(int c, io_t *io)
{
	io_putc(c & 0xFF, io);
}

static Filter_t ftab[] = {
	{ "plain", "Plain-Ausgabe", plain_putc },
	{ "TeX", "TeX-Filter", tex_putc },
};

Filter_t *GetFilter (const char *name)
{
	int i;

	for (i = 0; i < tabsize(ftab); i++)
		if	(mstrcmp(name, ftab[i].name) == 0)
			return ftab + i;

	return NULL;
}
