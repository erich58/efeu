/*
Test auf ein Schlüselwort in einem String

$Copyright (C) 2008 Erich Frühstück
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

#include <EFEU/mstring.h>
#include <ctype.h>

#define	IS_DELIM(c)	((c) != '_' && !isalnum((unsigned char) (c)))

int mtestkey (const char *def, char **ptr, const char *key)
{
	int flag;

	if	(ptr)
		*ptr = (char *) def;

	if	(key == NULL)
		return def ? 0 : 1;

	if	(def == NULL)
		return 0;

	while (*def == ' ' || *def == '\t')
		def++;

	for (flag = 0; *def; def++, key++)
	{
		if	(IS_DELIM(*def))	break;
		if	(*key == ':')		flag = 1, key++;
		if	(*key == '*')		flag = 1, key--;
		if	(*key == '?')		;
		if	(*def != *key)		return 0;
	}

	if	(flag || *key == 0 || *key == ':' || *key == '*')
	{
		if	(ptr)
			*ptr = (char *) def;

		return 1;
	}

	return 0;
}
