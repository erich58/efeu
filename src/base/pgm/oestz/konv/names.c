/*	Gegenstands/Kriterienbezeichner bestimmen
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include "oestz.h"


void oestz_names(OESTZ_HDR *hdr)
{
	char *p, *s;
	int i, n;

	do	p = oestz_get(0, 0);
	while	(p != NULL && *p != 'D');

	if	(p == NULL)
	{
		liberror(OESTZ, 3);
		return;
	}

	for (i = 0; i < hdr->gdim; i++)
	{
		if	(i % 7 == 0)
		{
			p = oestz_get('N', 1);
			n = 2;
		}

		s = oestz_parse(p, n, n + 4);

		while (*s == ' ')
			s++;

		hdr->gname[i] = mstrcpy(s);
		n += 5;
	}

	for (i = 0; i < hdr->kdim; i++)
	{
		if	(i % 17 == 0)
		{
			p = oestz_get('C', 1);
			n = 2;
		}

		hdr->kname[i] = oestz_cpy(p, n, n + 2);
		n += 3;
	}
}
