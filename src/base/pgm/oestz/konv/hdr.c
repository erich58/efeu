/*	Identifikation einmes Aggregat-Datenfiles
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include "oestz.h"


OESTZ_HDR *oestz_hdr(void)
{
	OESTZ_HDR *hdr;	/* Datenheader */
	size_t i;	/* Hilfszaehler */
	char *p;	/* Hilfspointer */

/*	Titel des Aggregatfiles
*/
	hdr = ALLOC(1, OESTZ_HDR);
	hdr->kennung = oestz_cpy(oestz_get('B', 1), 2, 6);
	hdr->erhebung = oestz_cpy(oestz_get('H', 1), 10, 54);
	hdr->objekt = oestz_cpy(oestz_get('L', 1), 10, 54);

/*	Zeitbereich ignorieren
*/
	(void) oestz_get('Z', 1);
	(void) oestz_get('3', 1);
	(void) oestz_get('3', 0);	/* Optional */

/*	Selektorzahl, 'M' - Saetze werden ueberlesen
*/
	p = oestz_get('S', 1);
	hdr->sdim = atoi(oestz_parse(p, 2, 6));

	for (i = 0; i < hdr->sdim; i += 6)
		(void) oestz_get('M', 1);

/*	Zahl der Kriterien und Objektbeschreibungen lesen
*/
	p = oestz_get('X', 1);
	hdr->gdim = atoi(oestz_parse(p, 2, 6));
	hdr->kdim = atoi(oestz_parse(p, 7, 11));
	hdr->gname = ALLOC(hdr->gdim, char *);
	hdr->kname = ALLOC(hdr->kdim, char *);
	return hdr;
}
