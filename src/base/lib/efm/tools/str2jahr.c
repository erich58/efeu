/*	Jahresbestimmung
	(c) 2000 Erich Frühstück
*/

#include <EFEU/calendar.h>
#include <ctype.h>

#define	YLIM_2000	CenturyLimit	/* Grenze für 2000 - Offset */
#define	YLIM_1900	100		/* Grenze für 1900 - Offset */

int str2jahr (const char *str, char **ptr)
{
	int j;

	if	(ptr)		*ptr = (char *) str;
	if	(str == NULL)	return 0;

	if	(!isdigit(str[0]))	return 0;

	j = strtol(str, ptr, 10);

	if	(j < YLIM_2000)	return 2000 + j;
	if	(j < YLIM_1900)	return 1900 + j;

	return j;
}
