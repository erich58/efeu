/*	Suchtabelle um Eintr�ge erweitern
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>


void xappend(xtab_t *tab, void *base, size_t nel, size_t width, int flag)

{
	char *ptr;	/* Hilfspointer */
	int i;		/* Hilfsz�hler */

	ptr = (char *) base;

	for (i = 0; i < nel; i++)
	{
		(void) xsearch(tab, (void *) ptr, flag);
		ptr += width;
	}
}
