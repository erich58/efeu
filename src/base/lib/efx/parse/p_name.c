/*
Namen lesen

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

#include <EFEU/object.h>
#include <EFEU/Op.h>
#include <ctype.h>

#define	NAME_KEY	"operator"

char *Parse_name (IO *io, int flag)
{
	void *name;

	if	((name = io_getname(io)) == NULL)
	{
		if	(flag)
			io_error(io, "[efmain:81]", NULL);

		return NULL;
	}

	if	(strcmp(name, NAME_KEY) == 0)
	{
		memfree(name);

		if	(io_eat(io, " \t") == '"')
		{
			io_scan(io, SCAN_STR, &name);
		}
		else	name = io_mgets(io, "%s");
	}

	return name;
}
