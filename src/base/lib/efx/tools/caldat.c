/*	Test auf kalendarisches Datum
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/


#include <EFEU/datum.h>


/*	Zahl der Tage eines Monats
*/

static int tage[13] = {
	29,	/* Februar, wenn Schaltjahr */
	31,	/* Jänner */
	28,	/* Februar */
	31,	/* März */
	30,	/* April */
	31,	/* Mai */
	30,	/* Juni */
	31,	/* Juli */
	31,	/* August */
	30,	/* September */
	31,	/* Oktober */
	30,	/* November */
	31,	/* Dezember */
};


/*	Die Funktion liefert 0 bei korrektem Datum, 1 bei falscher
	Tagesangabe und 2 bei falscher Monatsangabe. Der
	Jahrhundertausgleich (entfällt im Jahr 2000!) ist
	nicht berücksichtigt.
*/

int TestDat(OldDatum_t dat)
{
	int x;

	if	(dat.monat < 1 || dat.monat > 12)	return 1;

	x = (dat.monat == 2 && dat.jahr % 4 == 0) ? 0 : dat.monat;

	if	(dat.tag < 1 || dat.tag > tage[x])	return 2;

	return 0;
}
