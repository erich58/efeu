/*	Datumsangabe aus String gewinnen
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/datum.h>

OldDatum_t Datum(int tag, int monat, int jahr)
{
	OldDatum_t x;
	x.jahr = (jahr > 100 ? jahr : jahr + 1900);
	x.monat = monat;
	x.tag = tag;
	return x;
}

OldDatum_t str2Dat(const char *str)
{
	OldDatum_t x;
	int t, m, j;

	t = m = j = 0;
	sscanf((char *) str, "%d.%d.%d", &t, &m, &j);
	x.jahr = (j > 100 ? j : j + 1900);
	x.monat = m;
	x.tag = t;
	return x;
}

char *Dat2str(OldDatum_t dat)
{
	static char buf[12];

	sprintf(buf, "%2d.%2d.%4d", dat.tag, dat.monat, dat.jahr);
	return (char *) &buf;
}
