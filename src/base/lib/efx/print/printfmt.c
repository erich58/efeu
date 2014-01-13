/*
Datenwerte formatieren

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
#include <EFEU/printobj.h>
#include <EFEU/fmtkey.h>

/*	Abfragefunktionen
*/

int PrintFmtObj (IO *io, const char *fmt, const EfiObj *obj)
{
	if	(io && fmt)
	{
		EfiObjList *list;
		StrBuf *sb;
		int n;

		list = NewObjList(RefObj(obj));
		sb = sb_acquire();
		StrBufFmtList(sb, fmt, list);
		n = io_wputs(sb_nul(sb), io);
		sb_release(sb);
		DelObjList(list);
		return n;
	}

	return 0;
}


/*	Formatierungsfunktion
*/

int PrintFmtList (IO *io, const char *fmt, EfiObjList *list)
{
	if	(io && fmt)
	{
		StrBuf *sb;
		int n;

		sb = sb_acquire();
		StrBufFmtList(sb, fmt, list);
		n = io_wputs(sb_nul(sb), io);
		sb_release(sb);
		return n;
	}

	return 0;
}
