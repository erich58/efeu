/*
Teilstring abtrennen

$Copyright (C) 1996 Erich Frühstück
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

/*
Die Funktion |$1| trennt aus dem String <str> den mit einem Zeichen von
<delim> abgeschlossenen Teilstring ab. Falls <ptr> verschieden von NULL ist,
wird dieser auf das erste Zeichen des Reststrings gesetzt. Falls <flag>
verschieden von 0 ist, wird ein mehrfaches Vorkommen von Trennsymbolen
aus <delim> wie ein einzelnes Trennsymbol behandelt.
Falls für <delim> ein Nullpointer übergeben wird, gelten 
Leerzeichen, Tabulatoren und Zeilenvorschübe als Trennzeichen.
*/

char *mstrcut (const char *str, char **ptr, char *delim, int flag)
{
	int i;

	if	(!str)	return NULL;
	
	if	(!delim)	delim = " \t\n";

	for (i = 0; str[i] != 0; i++)
	{
		if	(str[i] == '\\' && str[i + 1] != 0)
		{
			i++;
		}
		else if	(strchr(delim, str[i]))
		{
			char *res = i ? mstrncpy(str, i) : NULL;

			if	(ptr)
			{
				i++;

				if	(flag)
					while (str[i] && strchr(delim, str[i]))
						i++;

				*ptr = (char *) str + i;
			}

			return res;
		}
	}

	if	(ptr)	*ptr = NULL;

	return mstrcpy(str);
}

