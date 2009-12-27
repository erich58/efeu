/*	Suchtabelle um Einträge erweitern
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>


void xappend(xtab_t *tab, void *base, size_t nel, size_t width, int flag)

{
	char *ptr;	/* Hilfspointer */
	int i;		/* Hilfszähler */

	ptr = (char *) base;

	for (i = 0; i < nel; i++)
	{
		(void) xsearch(tab, (void *) ptr, flag);
		ptr += width;
	}
}
