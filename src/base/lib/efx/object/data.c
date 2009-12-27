/*
Datenwerte löschen/kopieren/verschieben

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

#include <EFEU/object.h>


void CleanData(const EfiType *type, void *tg)
{
	if	(type->fclean)
		type->fclean->eval(type->fclean, NULL, &tg);

	if	(type->clean)	type->clean(type, tg);
	else			memset(tg, 0, type->size);
}

void CopyData(const EfiType *type, void *tg, const void *src)
{
	if	(type->copy)	type->copy(type, tg, src);
	else			memcpy(tg, src, type->size);

	if	(type->fcopy)
	{
		void *arg = (void *) src;
		type->fcopy->eval(type->fcopy, tg, &arg);
	}
}


void AssignData(const EfiType *type, void *tg, const void *src)
{
	if	(tg == src)	return;

	CleanData(type, tg);
	CopyData(type, tg, src);
}


/*	Vektordaten
*/

void CleanVecData(const EfiType *type, size_t dim, void *tg)
{
	while (dim > 0)
	{
		CleanData(type, tg);
		tg = ((char *) tg) + type->size;
		dim--;
	}
}


void CopyVecData(const EfiType *type, size_t dim, void *tg, const void *src)
{
	while (dim > 0)
	{
		CopyData(type, tg, src);
		tg = ((char *) tg) + type->size;
		src = ((char *) src) + type->size;
		dim--;
	}
}


void AssignVecData(const EfiType *type, size_t dim, void *tg, const void *src)
{
	if	(tg != src)
	{
		CleanVecData(type, dim, tg);
		CopyVecData(type, dim, tg, src);
	}
}
