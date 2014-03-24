/*
Zufallszahlengenerator mit linearer Kongruenz und 48 Bit Arithmetik

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
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>

#define	S0	0x330E

#define	A2	0x0005
#define	A1	0xDEEC
#define	A0	0xE66D

#define	C	0x000B

#define	M	(1. / 0x10000)

typedef struct {
	unsigned short w2;
	unsigned short w1;
	unsigned short w0;
} D48;

static void ident_d48 (IO *io, void *data)
{
	D48 *d48 = data;
	io_xprintf(io, "0x%04X%04X%04X", d48->w2, d48->w1, d48->w0);
}

static void seed_d48 (void *data, unsigned int x)
{
	D48 *d48 = data;
	d48->w2 = (x >> 16);
	d48->w1 = x & 0xFFFF;
	d48->w0 = S0;
}

static void *init_d48 (unsigned int x)
{
	void *state = memalloc(sizeof(D48));
	seed_d48(state, x);
	return state;
}

static void *copy_d48 (const void *src)
{
	return memcpy(memalloc(sizeof(D48)), src, sizeof(D48));
}

static double rand_d48 (void *data)
{
	D48 *d48 = data;
	unsigned r0, r1, r2;

	r0 = C + d48->w0 * A0;
	r1 = (r0 >> 16) + d48->w0 * A1;
	r2 = (r1 >> 16) + d48->w0 * A2;
	r1 = (r1 & 0xFFFF) + d48->w1 * A0;
	r2 = (r2 & 0xFFFF) + (r1 >> 16) + d48->w1 * A1 + d48->w2 * A0;
	d48->w0 = (r0 & 0xFFFF);
	d48->w1 = (r1 & 0xFFFF);
	d48->w2 = (r2 & 0xFFFF);
	return ((d48->w0 * M + d48->w1) * M + d48->w2) * M;
}

RandomType RandomType_d48 = {
	"d48", ":*:drand48 random number generator:de:drand48 Zufallsgenerator",
	ident_d48, init_d48, copy_d48, memfree, seed_d48, rand_d48
};
