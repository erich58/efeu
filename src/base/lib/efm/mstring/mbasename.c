/*
:*:Return dirname/basename of given pathname
:de:Verzeichnisname/Basisname aus Pfadnamens generieren

$Copyright (C) 2002 Erich Frühstück
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

#include <EFEU/mstring.h>

#define	PART_DIR	0
#define	PART_NAME	1
#define	PART_XDIR	2

static char *partname (const char *path, int mode)
{
	int start, end;

	if	(path == NULL || path[0] == '\0')
		return NULL;

	end = strlen(path) - 1;

	while (path[end] == '/')
	{
		if (end == 0)
			return mstrncpy(path, 1);
		
		end--;
	}

	start = end;
	
	while (path[start] != '/')
	{
		if	(start == 0)
			return mode == PART_NAME ?
				mstrncpy(path, end + 1) : NULL;

		start--;
	}

	if	(mode == PART_NAME)
		return mstrncpy(path + start + 1, end - start);

	while (path[start] == '/')
	{
		if (start == 0)
			return mstrncpy(path, 1);
		
		start--;
	}

	return mstrncpy(path, start + (mode == PART_XDIR ? 2 : 1));
}


/*
:*:
The function |$1| returns the directory part of the filename <path>.
If flag is set to true, the return value contains a final '/'.
If there is no directory part, the function returns |NULL| (not "." or "./").
:de:
Die Funktion |$1| liefert den Verzeichnisteil des Filenamens <path>.
Falls <flag> verschieden von 0 ist, enthält der Verzeichnisteil
ein abschließendes '/'.
Falls <path> keine Verzeichnisse enthält, liefert die Funktion
|NULL| (nicht "." oder "./").
*/

char *mdirname (const char *path, int flag)
{
	return partname(path, flag ? PART_XDIR : PART_DIR);
}

/*
:*:
The function |$1| returns the name of the file <path> with all leading
directory components removed. if <suffix> is not |NULL|, the filesuffix
is detached from the basename and stored in <suffix>.
:de:
Die Funktion |$1| liefert den Basisnamen einer Datei. Falls für <suffix>
kein Nullpointer übergeben wurde, wird der Filezusatz abgetrennt und in
<suffix> gespeichert.
*/

char *mbasename (const char *path, char **suffix)
{
	char *name = partname(path, PART_NAME);

	if	(suffix)
	{
		*suffix = NULL;

		if	(name)
		{
			char *p = strrchr(name, '.');

			if	(p && p != name)
			{
				*p = 0;
				*suffix = p + 1;
			}
		}
	}

	return name;
}

/*
$Note
:*:
The functions calls |memalloc| to allocate memory for the return value.
The address stored in <suffix> is part of the memory space returned
by |basename|. It should not used as argument to |memfree| and is
invalid, if the return value of |basename| is free'd.
:de:
Das Speicherfeld für die Rückgabewerte wird mit |memalloc| eingerichtet.
Der Pointer auf <suffix> gehört zum Speicherfeld für den Basisnamen.
Er darf nicht als Argument von |memfree| verwendet werden und ist
ungültig, sobald der Rückgabewert von |basename| freigegeben wird.

$SeeAlso
\mref{memalloc(3)}.\br
\mref{string(3C)} @PRM.
*/
