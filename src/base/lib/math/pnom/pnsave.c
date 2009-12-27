/*	Ausgabe einer Polynomstruktur in gepackten Format
	(c) 1991 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <Math/pnom.h>


typedef struct {
	ushort_t dim;
	ushort_t deg;
} HEADER;


void pnsave(pnom_t *p, const char *name)
{
	io_t *aus;
	HEADER hdr;

	aus = io_fileopen(name, "wb");

	if	(p == NULL)
	{
		hdr.dim = 0;
		hdr.deg = 0;
		io_write(aus, (void *) &hdr, sizeof(HEADER));
		io_close(aus);
		return;
	}

	hdr.dim = p->dim;
	hdr.deg = p->deg;
	io_write(aus, (void *) &hdr, sizeof(HEADER));
	io_write(aus, p->x, sizeof(double) * p->dim * (p->deg + 2));
	io_close(aus);
}


pnom_t *pnload(const char *name)
{
	pnom_t *p;
	io_t *ein;
	HEADER hdr;

	ein = io_fileopen(name, "rb");

	if	(io_read(ein, &hdr, sizeof(HEADER)) != sizeof(HEADER))
	{
		io_close(ein);
		return NULL;
	}
	
	p = pnalloc(hdr.dim, hdr.deg);
	p->dim = hdr.dim;
	p->deg = hdr.deg;
	io_read(ein, p->x, sizeof(double) * p->dim * (p->deg + 2));
	io_close(ein);
	return p;
}
