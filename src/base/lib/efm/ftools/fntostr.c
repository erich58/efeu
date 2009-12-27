/*	Umwandeln einer fname_t - Struktur in einen Filenamen
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>

char *fntostr (const fname_t *fname)
{
	char *p[5];
	size_t n;

	if	(fname == NULL || fname->name == NULL)
	{
		return NULL;
	}

	n = 0;

	if	(fname->path != NULL)
	{
		p[n++] = fname->path;
		p[n++] = "/";
	}

	p[n++] = fname->name;

	if	(fname->type != NULL)
	{
		p[n++] = ".";
		p[n++] = fname->type;
	}

	return listcat(NULL, p, n);
}
