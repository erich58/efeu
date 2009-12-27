/*	Sekundenintervall bestimmen
	(c) 2009 Erich Frühstück
*/

#include <EFEU/calendar.h>
#include <ctype.h>

int str2sec (const char *def, char **ptr)
{
	int sec;
	char *p;

	if	(!ptr)	ptr = &p;

	*ptr = (char *) def;
	sec = 0;

	if	(!def)	return 0;

	while (isdigit((unsigned char) *def))
	{
		int n = strtoul(def, ptr, 10);

		switch (**ptr)
		{
		case 'd':	sec += n * 24 * 60 * 60; break;
		case 'h':	sec += n * 60 * 60; break;
		case 'm':	sec += n * 60; break;
		case 's':	sec += n; break;
		default:	return sec + n;
		}

		def = ++(*ptr);
	}

	return sec;
}
