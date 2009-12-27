/*
Aufspalten einer IO-Struktur in Teilstrings

$Copyright (C) 1994 Erich Frühstück
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


size_t io_split(IO *io, const char *delim, char ***ptr)
{
	IO *tmp;
	size_t size;
	size_t i, dim;
	char *p;
	int c;

	if	(ptr == NULL)
	{
		return 0;
	}
	else	*ptr = NULL;

	tmp = io_tmpbuf(0);
	size = 0;
	dim = 0;

/*	Teilstrings zwischenspeichern
*/
	while ((c = io_eat(io, delim)) != EOF)
	{
		p = io_mgets(io, delim);
		size += io_savestr(tmp, p);
		memfree(p);
		dim++;
	}

/*	Stringliste generieren
*/
	*ptr = (char **) memalloc(size + dim * sizeof(char *));
	p = (char *) ((*ptr) + dim);
	io_rewind(tmp);

	for (i = 0; i < dim; i++)
	{
		(*ptr)[i] = io_loadstr(tmp, &p);
	}

	io_close(tmp);
	return dim;
}
