/*
Auf random(3) beruhende Zufallszahlengeneratoren

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/Random.h>

static void ident_rx (io_t *io, unsigned int *ptr, size_t n)
{
	io_printf(io, "data[%d] =", n);

	for (n /= sizeof(unsigned int); n-- > 0; ptr++)
		io_printf(io, " %08x", *ptr);
}


static void *init_rx (unsigned int sval, size_t size)
{
	char *data = memalloc(size);
	char *save = initstate(sval, data, size);

	if	(save)	setstate(save);

	return data;
}


static void seed_rx (void *data, unsigned int sval)
{
	void *save = (void *) setstate(data);
	srandom(sval);

	if	(save) setstate(save);
}

static double rand_rx (void *data)
{
	void *save = (void *) setstate(data);
	double x = DRAND_KOEF * random();

	if	(save) setstate(save);

	return x;
}

/*	Zufallszahlengeneratortypen einrichten
*/

#define	RTYPE(name, desc, size)	\
static void ident_ ## name (io_t *io, void *data) \
{ ident_rx (io, data, size); }; \
static void *init_ ## name (unsigned int sval) \
{ return init_rx (sval, size); }; \
static void *copy_ ## name (const void *src) \
{ return memcpy(memalloc(size), src, size); } \
RandomType_t RandomType_ ## name = { \
	#name, desc, ident_ ## name, \
	init_ ## name, copy_ ## name, memfree, \
	seed_rx, rand_rx \
};


RTYPE(r0, "random(3), state size = 8", 8)
RTYPE(r1, "random(3), state size = 32", 32)
RTYPE(r2, "random(3), state size = 64", 64)
RTYPE(r3, "random(3), state size = 128", 128)
RTYPE(r4, "random(3), state size = 256", 256)

