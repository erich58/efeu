/*
Lokale-Setzung

$Copyright (C) 2007 Erich Frühstück
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

#include <EFEU/procenv.h>
#include <EFEU/locale.h>
#include <EFEU/mstring.h>

char *CurrentLocale = NULL;

void ChangeLocale (const char *def)
{
	if	(def)
	{
		if	(mstrcmp(CurrentLocale, def) == 0)
			return;
	}
	else if	(CurrentLocale)
	{
		return;
	}

	memfree(CurrentLocale);
	CurrentLocale = mstrcpy(setlocale(LC_ALL, def ? def : ""));
}
