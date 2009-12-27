/*	Vergleich zweier Strings mit Test auf Nullpointer
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/mstring.h>

static int do_cmp (const uchar_t *a, const uchar_t *b)
{
	while (*a || *b)
	{
		if	(*a < *b)	return -1;
		if	(*a > *b)	return 1;

		a++;
		b++;
	}

	return 0;
}

int mstrcmp (const char *a, const char *b)
{
	if	(a == b)	return 0;
	else if	(a == NULL)	return -1;
	else if	(b == NULL)	return 1;

	return do_cmp((const uchar_t *) a, (const uchar_t *) b);
}

size_t mstrlen (const char *s)
{
	return s ? strlen(s) : 0;
}
