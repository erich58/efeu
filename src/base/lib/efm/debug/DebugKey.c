/*
Kennungen für Debuglevel

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

#include <EFEU/Debug.h>
#include <EFEU/mstring.h>

static char *debug_key[DBG_DIM] = {
	"none",
	"err",
	"note",
	"stat",
	"trace",
	"debug",
};

/*
Die Funktion |$1| konvertiert eine Debug-Kennung in einen Debug-Level.
Folgende Kennungen sind definiert: none, err, note, stat, trace, debug.
*/

int DebugKey (const char *name)
{
	int i;
	
	for (i = 0; i < DBG_DIM; i++)
		if (mstrcmp(debug_key[i], name) == 0)
			return i;

	return 0;
}

/*
Die Funktion |$1| liefert die Kennung eines Debug-Levels.
*/

char *DebugLabel (int type)
{
	if	(type < 0)		type = 0;
	if	(type >= DBG_DIM)	type = DBG_DIM - 1;

	return debug_key[type];
}

