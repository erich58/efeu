/*	Selektion mit Musterliste
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/patcmp.h>


int patselect(const char *name, char **list, size_t dim)
{
	int flag;	/* Selektionsflag */
	int mode;	/* Selektionstyp */
	char *pat;	/* Selektionsmuster */
	int i;		/* Hilfszähler */

	flag = 1;

	for (i = 0; i < dim; i++)
	{
		pat = list[i];

		switch (*pat)
		{
		case '+':	mode = 1; pat++; break;
		case '-':	mode = 0; pat++; break;
		default:	mode = 1; break;
		}

		if	(i > 0 && mode == flag)	continue;

		flag = (patcmp(pat, name, NULL) ? mode : !mode);
	}

	return flag;
}
