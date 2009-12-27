/*	Umwandeln eines Wertes in Ziffern der Basis 37
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/efutil.h>

static char *digit = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char buf[8];	/* Statischer Buffer */

char *l37a (unsigned x)
{
	char *p;

	p = buf + 7;
	*p = 0;

	while (x != 0)
	{
		*(--p) = digit[x % 37];
		x /= 37;
	}

	return p;
}
