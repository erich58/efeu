/*	Skalierung fuer Gegenstaende
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include "oestz.h"

double *oestz_scale(OESTZ_HDR *hdr, const char *def)
{
	int i, j, k;
	double *skale;
	double x;
	size_t n;
	char **list;

	if	(hdr->gdim == 0)
	{
		return NULL;
	}

	skale = ALLOC(hdr->gdim, double);
	n = strsplit(def, "%s", &list);

	for (i = 0; i < hdr->gdim; i++)
		skale[i] = 1.;

	for (i = 0; i < n; i++)
	{
		x = 1.;

		for (j = 0; list[i][j] != 0; j++)
		{
			if	(list[i][j] == '=')
			{
				x = atof(list[i] + j + 1);
				break;
			}
		}

		for (k = 0; k < hdr->gdim; k++)
		{
			if	(strncmp(list[i], hdr->gname[k], j) == 0)
			{
				skale[k] = x;
			}
		}
	}

	return skale;
}
