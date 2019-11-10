/*
Kopierhilfsfunktionen

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
#include <EFEU/strbuf.h>
#include <EFEU/efio.h>

void copy_str (IO *ein, IO *aus, int delim)
{
	int c, flag;

	flag = 0;
	io_protect(ein, 1);

	while ((c = io_getc(ein)) != EOF)
	{
		io_putc(c, aus);

		if	(flag)		flag = 0;
		else if	(c == delim)	break;
		else if	(c == '\\')	flag = 1;
	}

	io_protect(ein, 0);
}

void copy_token (IO *ein, IO *aus, int c, int flag)
{
	if	(!(flag && isspace(c)))
		io_putc(c, aus);

	switch (c)
	{
	case '"':
	case '\'':	copy_str(ein, aus, c); break;
	case '{':	copy_block(ein, aus, '}', 0); break;
	case '(':	copy_block(ein, aus, ')', 1); break;
	case '[':	copy_block(ein, aus, ']', 1); break;
	default:	break;
	}
}

void copy_block (IO *ein, IO *aus, int end, int flag)
{
	char *prompt;
	int c, last;

	prompt = io_prompt(ein, ">>> ");
	last = 0;

	while ((c = io_getc(ein)) != EOF && c != end)
	{
		copy_token(ein, aus, c, (flag && last == '\n'));
		last = c;
	}

	io_putc(end, aus);
	io_prompt(ein, prompt);
}

