/*
Lesehilfsfunktionen

$Copyright (C) 2000 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include "src2doc.h"
#include <ctype.h>

int skip_blank (io_t *ein)
{
	int c;

	while ((c = io_skipcom(ein, NULL, 0)) != EOF)
		if (c != ' ' && c != '\t') return c;

	return EOF;
}

int skip_space (io_t *ein, strbuf_t *buf)
{
	int c;

	while ((c = io_skipcom(ein, buf, 1)) != EOF)
		if (!isspace(c)) return c;

	return EOF;
}


int test_key (io_t *io, const char *key)
{
	int i;
	int c;

	if	(key == NULL)	return 0;

	for (i = 0; ; i++)
	{
		c = io_getc(io);

		if	(key[i] == 0 || c != key[i])	break;
	}

	io_ungetc(c, io);

	if	(key[i] == 0)	return 1;

	while (i-- > 0)
		io_ungetc(key[i], io);

	return 0;
}
