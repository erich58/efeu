/*	Vektoren aufspalten
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/vecbuf.h>
#include <EFEU/memalloc.h>

size_t vsplit (void *base, size_t dim, size_t size, test_t test)
{
	size_t i0, i1;
	char *p0, *p1;

	if	(dim == 0)	return 0;
	if	(test == NULL)	return dim;

	i0 = 0;
	p0 = base;
	i1 = dim - 1;
	p1 = p0 + i1 * size;

	do
	{
		while (i0 <= i1 && test(p0))
			i0++, p0 += size;

		while (i1 > i0 && !test(p1))
			i1--, p1 -= size;

		if	(i0 < i1)
		{
			memswap(p0, p1, size);
			p0 += size;
			p1 -= size;
			i0++;
			i1--;
		}
		else	break;
	}
	while (i0 <= i1);

	return i0;
}
