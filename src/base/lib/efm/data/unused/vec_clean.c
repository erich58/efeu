/*	Vektoren löschen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/data.h>

void vec_clean(clean_t clean, void *data, size_t size, size_t dim)
{
	if	(clean)
	{
		register char *p;

		for (p = data; dim > 0; dim--, p += size)
			clean(p);
	}
	else	memset(data, 0, size * dim);
}
