/*
Von libc6-1.0.3 aus random_r.c abgeleiteter Code

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

#define	BREAK		128
#define	DEG		31
#define	SEP		3

typedef struct {
	int *fptr;	/* Front Pointer */
	int *rptr;	/* Rear Pointer */
	int *end_ptr;	/* Pointer am  Pointer */
	int state[DEG];	/* Statusfeld */
} RNDBUF;

static void ident_old (IO *io, void *data)
{
	RNDBUF *buf = data;
	int n;
	
	io_xprintf(io, "%2d", buf->fptr - buf->state);
	io_xprintf(io, "%2d", buf->rptr - buf->state);

	for (n = 0; n < DEG; n++)
		io_xprintf(io, " %08x", buf->state[n]);
}

/*
   The basic operation is to add the number at the rear pointer into
   the one at the front pointer.  Then both pointers are advanced to the next
   location cyclically in the table.  The value returned is the sum generated,
   reduced to 31 bits by throwing away the "least random" low bit.
   Note: The code takes advantage of the fact that both the front and
   rear pointers can't wrap on the same call by not testing the rear
   pointer if the front one has wrapped.  Returns a 31-bit random number. 
*/

static int do_rand (RNDBUF *buf)
{
	int result;

	*buf->fptr += *buf->rptr;
	/* Chucking least random bit.  */
	result = (*buf->fptr >> 1) & 0x7fffffff;
	++buf->fptr;

	if	(buf->fptr >= buf->end_ptr)
	{
		buf->fptr = buf->state;
		++buf->rptr;
	}
	else
	{
		++buf->rptr;

		if (buf->rptr >= buf->end_ptr)
			buf->rptr = buf->state;
	}

	return result;
}

static double rand_old (void *data)
{
	return do_rand(data) / 2147483648.;
}


/*	Initializes state[] based on the given "seed" via a linear
	congruential generator.  Then, the pointers are set to known
	locations that are exactly SEP places apart. 
	Lastly, it cycles the state information a given number of times
	to get rid of any initial dependencies introduced by the L.C.R.N.G.
*/

static void seed_old (void *data, unsigned int x)
{
	RNDBUF *buf;
	long int i;
	
	buf = data;

/*	We must make sure the seed is not 0.
	Take arbitrarily 1 in this case. 
*/
	buf->state[0] = x ? x : 1;

	for (i = 1; i < DEG; ++i)
	{
		/* This does:
		state[i] = (16807 * state[i - 1]) % 2147483647;
		but avoids overflowing 31 bits.  */
		long int hi = buf->state[i - 1] / 127773;
		long int lo = buf->state[i - 1] % 127773;
		long int test = 16807 * lo - 2836 * hi;
		buf->state[i] = test + (test < 0 ? 2147483647 : 0);
	}

	buf->fptr = &buf->state[SEP];
	buf->rptr = &buf->state[0];
	buf->end_ptr = &buf->state[DEG];

	for (i = 0; i < 10 * DEG; ++i)
		do_rand(buf);
}

static void *init_old (unsigned int x)
{
	RNDBUF *buf = memalloc(sizeof(RNDBUF));
	seed_old(buf, x);
	return buf;
}


static void *copy_old (const void *data)
{
	const RNDBUF *src = data;
	RNDBUF *tg = memalloc(sizeof(RNDBUF));
	memcpy(tg, src, sizeof(RNDBUF));
	tg->fptr = tg->state + (src->fptr - src->state);
	tg->rptr = tg->state + (src->rptr - src->state);
	tg->end_ptr = tg->state + DEG;
	return tg;
}

RandomType RandomType_old = {
	"old", ":*:compatible to:de:Kompatibel zu:_: random(3), libc6-2.0.3",
	ident_old, init_old, copy_old, memfree, seed_old, rand_old
};
