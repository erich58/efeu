/*	Aufspalten eines Strings
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>

char *nextstr (char **ptr)
{
	char *p = *ptr;

	while (**ptr != 0)
		(*ptr)++;

	(*ptr)++;
	return (*p == 0 ? NULL : p);
}

size_t strsplit(const char *str, const char *delim, char ***ptr)
{
	size_t size;
	size_t dim;
	char *p;

	if	(ptr == NULL)	return 0;
	else			*ptr = NULL;

	if	(str == NULL)	return 0;

/*	Zahl der Teilstringe und Speicherplatz bestimmen
*/
	for (size = 0, dim = 1; str[size] != 0; size++)
	{
		if	(listcmp(delim, str[size]))
		{
			dim++;
		}
	}

/*	Initialisieren der Teilstringstruktur
*/
	size += 1 + dim * sizeof(char *);
	*ptr = (char**) memalloc((ulong_t) size);
	(*ptr)[0] = p = (char *) ((*ptr) + dim);

/*	String in Teilstringe umspeichern
*/
	for (size = 0, dim = 1; str[size] != 0; size++)
	{
		if	(listcmp(delim, str[size]))
		{
			*(p++) = 0;
			(*ptr)[dim++] = p;
		}
		else	*(p++) = str[size];
	}

	*(p) = 0;
	return dim;
}
