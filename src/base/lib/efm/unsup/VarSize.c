/*	Längenangabe mit größenabhängiger Satzlänge
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/String.h>

#define	ERR	"sorry: VarSize(%u) too big\n"

#define	BASE	6	/* Basisstellen des ersten Bytes */
#define	BMASK	077	/* Maske für Basisstellen */
#define	MAXEXP	4	/* Maximaler Exponent */

static unsigned get_exp (unsigned size)
{
	register unsigned exp;
	register unsigned lim;

	for (exp = 0, lim = 1 << BASE; exp < MAXEXP; exp++, lim <<= 8)
		if	(size < lim)	return exp;

	fprintf(stderr, ERR, size);
	exit(EXIT_FAILURE);
	return exp;
}

size_t VarSize_iofunc (iofunc_t func, void *ptr, VarSize_t *size)
{
	unsigned recl, i, exp, offset;
	uchar_t c;

/*	Exponent und Basisstellen verarbeiten
*/
	c = getexp(size) << BASE;
	c |= (size & BMASK);
	recl = func(ptr, &c, 1, 1);
	size &= BMASK;
	size |= (c & BMASK);
	exp = (c >> BASE);

/*	Restlichen Byte verarbeiten
*/
	for (i = 0, offset = BASE; i < exp; i++, offset += 8)
	{
		c = (size >> offset) & 0xFF;
		recl += func(ptr, &c, 1, 1);
		size &= ~(0xFF << offset);
		size |= (unsigned) c << offset;
	}

	return recl;
}

size_t VarSize_ioskip (iofunc_t func, void *ptr, VarSize_t *data)
{
	return func(ptr, data, 0, VarSize_size);
}
