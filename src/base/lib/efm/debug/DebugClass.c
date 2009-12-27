/*
:*:	debug class
:de:	Debugklasse

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

#include <EFEU/Debug.h>
#include <EFEU/ftools.h>

FILE *DebugClassFile (DebugClass *cl)
{
	if	(!cl)	return NULL;

	if	(cl->sync < DebugChangeCount)
	{
		cl->sync = DebugChangeCount;

		if	(cl->log)
		{
			fprintf(cl->log, "STOP debugging %s\n", cl->name);
			fileclose(cl->log);
		}

		cl->log = filerefer(LogFile(cl->name, cl->level));

		if	(cl->log)
			fprintf(cl->log, "START debugging %s\n", cl->name);
	}

	return cl->log;
}
