/*
:*:query format
:de:Formatabfrage

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/Resource.h>
#include <EFEU/CmdPar.h>
#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/Info.h>
#include <EFEU/Debug.h>
#include <ctype.h>

#define	SIGLEN	63

/*
Die Funktion |$1| sucht in der Formatdefinition <def> nach einer
Formatkennung und liefert das zugehörige Format. Falls <def>
keine Formatkennung enthält, liefert die Funktion <def>. Falls
die Formatdefinition nicht gefunden wurde, liefert die Funktion
den nach der Formatkennung folgenden Text. Eine Formatkennung
hat die Form <"|[|<name>|]|"> und muß am Anfang der Formatkennung
stehen. Die Bedeutung von |[| kann mit einem Backslash aufgehoben
werden.
*/

char *GetFormat (const char *def)
{
	char nbuf[SIGLEN + 1];
	const char *p;
	char *key;
	char *fmt;
	int flag;
	int i;

	if	(def == NULL)
		return NULL;

	if	(def[0] == '\\' && def[1] == '[')
		return (char *) def + 1;

	if	(*def != '[')
		return (char *) def;

	p = def + 1;
	flag = (*p == '%');

	if	(flag)	p++;

	key = flag ? nbuf : NULL;

	for (i = 0; *p && *p != ']'; p++)
	{
		if	(*p == 0)	return (char *) def;
		if	(i >= SIGLEN)	continue;

		if	(*p == ':' && key == NULL)
		{
			nbuf[i++] = 0;
			key = nbuf + i;
		}
		else	nbuf[i++] = *p;
	}

	nbuf[i] = 0;
	p++;

	if	(*p == 0)	p = NULL;

	if	(flag)
		return GetResource(nbuf, p);

	fmt = key ? FormatTabEntry(nbuf, key) : FormatTabEntry(NULL, nbuf);
	return fmt ? fmt : (char *) p;
}

/*
$SeeAlso
\mref{CmdPar(1)}.\br
*/

