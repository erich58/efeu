/*
Wörtwörtliche Ausgabe

$Copyright (C) 1999 Erich Frühstück
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

#include "efeudoc.h"
#include <ctype.h>

int DocVerb (IO *in, IO *out)
{
	int c, n;
	
	io_ctrl(out, DOC_BEG, DOC_ATT_TT);

	for (n = 0; (c = io_getucs(in)) != EOF; n++)
	{
		if	(c == '|')
		{
			c = io_getc(in);

			if	(c != '|')
			{
				io_ungetc(c, in);
				break;
			}
		}

		if	(isspace(c))
			io_putucs(' ', out);
		else	io_putucs(c, out);
	}

	io_ctrl(out, DOC_END, DOC_ATT_TT);
	return n;
}

