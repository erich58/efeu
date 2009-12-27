/*	Achsendimension bestimmen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>


/*	Achsendimension bestimmen
*/

size_t md_dim(mdaxis_t *x)
{
	int n;

	for (n = 0; x != NULL; n++)
		x = x->next;

	return n;
}


/*	Strukturgrößen berechnen
*/

size_t md_size(mdaxis_t *x, size_t size)
{
	if	(x != NULL)
	{
		x->size = md_size(x->next, size);
		return x->size * x->dim;
	}
	else	return size;
}
