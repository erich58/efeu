/*
Ausgabefunktionen

$Copyright (C) 1997 Erich Frühstück
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

#ifndef	_MS_PRINTFUNC
#define	_MS_PRINTFUNC	1

#include <function.ms>


void PrintFunc(str name, str args, IO def, bool glob = true)
{
	Function f = Function(name, paste(", ", "io_t *out", args));
	f.pfbody(def, "out", NULL);
	f.write(glob);
}

#endif	/* MS_PRINTFUNC */
