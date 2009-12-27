/*
Umwandeln eines Filenamens in eine fname_t - Struktur

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

#include <EFEU/ftools.h>

fname_t *strtofn(const char *name)
{
	fname_t *fname;
	char *p, *p1;
	size_t n;

	if	(name == NULL)	return NULL;

/*	Speicherplatz generieren und Filename umkopieren
*/
	n = sizeof(fname_t) + strlen(name) + 1;
	fname = (fname_t *) memalloc(n);
	p = strcpy((char *) (fname + 1), name);
	fname->path = NULL;
	fname->type = NULL;

/*	Pfadnamen abtrennen
*/
	p1 = strrchr(p, '/');

	if	(p1 != NULL && p1 != p)
	{
		*p1 = 0;
		fname->path = p;
		p = p1 + 1;
	}

/*	Filename initialisieren
*/
	fname->name = p;

/*	Filetype abtrennen
*/
	if	((p1 = strrchr(p, '.')) != NULL && p1 != p)
	{
		*p1 = 0;
		fname->type = p1 + 1;
	}

	return fname;
}
