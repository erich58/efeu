/*	Umwandeln eines Filenamens in eine fname_t - Struktur
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>

fname_t *strtofn(const char *name)
{
	fname_t *fname;
	char *p, *p1;
	size_t n;

	if	(name == NULL)	return NULL;

/*	Speicherplatz generieren und Filename umkopieren
*/
	n = sizeof(fname_t) + strlen(name) + 1;
	fname = (fname_t *) memalloc(n);
	p = strcpy((char *) (fname + 1), name);
	fname->path = NULL;
	fname->type = NULL;

/*	Pfadnamen abtrennen
*/
	p1 = strrchr(p, '/');

	if	(p1 != NULL && p1 != p)
	{
		*p1 = 0;
		fname->path = p;
		p = p1 + 1;
	}

/*	Filename initialisieren
*/
	fname->name = p;

/*	Filetype abtrennen
*/
	if	((p1 = strrchr(p, '.')) != NULL && p1 != p)
	{
		*p1 = 0;
		fname->type = p1 + 1;
	}

	return fname;
}
