/*
Stringvergleich alphabetischer Zeichen

$Copyright (C) 1999 Erich Frühstück
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

#include <EFEU/patcmp.h>
#include <ctype.h>

static int do_cmp (const unsigned char *a, const unsigned char *b)
{
	for (;;)
	{
		while	(*a && !isalpha(*a))
			a++;

		while	(*b && !isalpha(*b))
			b++;

		if	(*a < *b)	return -1;
		if	(*a > *b)	return 1;
		if	(*a == 0)	break;

		a++;
		b++;
	}

	return 0;
}


int alphacmp (const char *a, const char *b)
{
	if	(a == b)	return 0;
	if	(a == NULL)	return -1;
	if	(b == NULL)	return 1;

	return do_cmp((const unsigned char *) a, (const unsigned char *) b);
}
