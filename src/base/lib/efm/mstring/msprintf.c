/*
Printf f�r IO-Strukturen

$Copyright (C) 1994 Erich Fr�hst�ck
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
#include <EFEU/io.h>


char *msprintf(const char *fmt, ...)
{
	if	(fmt)
	{
		va_list args;
		char *p;

		va_start(args, fmt);
		p = mvsprintf(fmt, args);
		va_end(args);
		return p;
	}

	return NULL;
}


char *mvsprintf(const char *fmt, va_list list)
{
	if	(fmt)
	{
		StrBuf *sb = sb_acquire();
		sb_vxprintf(sb, fmt, list);
		return sb_cpyrelease(sb);
	}

	return NULL;
}
