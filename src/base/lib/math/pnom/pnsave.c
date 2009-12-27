/*
Ausgabe einer Polynomstruktur in gepackten Format

$Copyright (C) 1991 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
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
