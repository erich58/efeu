/*
Standardzufallsgenerator

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

static void ident_std (IO *io, void *data)
{
	io_xprintf(io, "%#08x", ((int *) data)[0]);
}

static void seed_std (void *data, unsigned int x)
{
	((unsigned int *) data)[0] = x ? x : 1;
}

static void *init_std (unsigned int x)
{
	void *state = memalloc(sizeof(int));
	seed_std(state, x);
	return state;
}

static void *copy_std (const void *src)
{
	return memcpy(memalloc(sizeof(int)), src, sizeof(int));
}

static double rand_std (void *data)
{
	int *state = data;
      	*state = ((*state * 1103515245) + 12345) & 0x7fffffff;
	return *state / 2147483648.;
}

RandomType RandomType_std = {
	"std", ":*:standard random number generator:de:Standardgenerator",
	ident_std, init_std, copy_std, memfree, seed_std, rand_std
};
