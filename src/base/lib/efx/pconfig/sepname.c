/*
Namen abtrennen

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

#include <EFEU/pconfig.h>

char *sepname(const char *str, char **ptr)
{
	int i;

	for (i = 0; str[i] != 0; i++)
	{
		if	(str[i] == '=')
		{
			if	(ptr)
			{
				if	(str[i+1] == 0)
				{
					*ptr = NULL;
				}
				else	*ptr = (char *) (str + i + 1);
			}

			if	(i != 0)
			{
				return mstrncpy(str, i);
			}
			else	return NULL;
		}
	}

	if	(ptr)
	{
		*ptr = (char *) str;
	}

	return NULL;
}
