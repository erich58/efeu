/*	Zusammenfügen einer Liste von Strings
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/mstring.h>


char *listcat (const char *delim, char **list, size_t dim)
{
	int i, j, n;
	char *x;

	if	(list == NULL || dim == 0)
		return NULL;

/*	Bestimmung des Speicherbedarfs
*/
	n = (delim != NULL ? (dim - 1) * strlen(delim) : 0);

	for (i = 0; i < dim; i++)
		if (list[i] != NULL) n += strlen(list[i]);

/*	Kopieren der Strings
*/
	x = memalloc(n + 1);
	n = 0;

	for (i = 0; i < dim; i++)
	{
		if	(i > 0 && delim != NULL)
		{
			for (j = 0; delim[j] != 0; j++)
				x[n++] = delim[j];
		}

		if	(list[i] != NULL)
		{
			for (j = 0; list[i][j] != 0; j++)
				x[n++] = list[i][j];
		}
	}

	x[n] = 0;
	return x;
}
