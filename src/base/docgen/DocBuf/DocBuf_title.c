/*
Titel und Copyright bestimmen

$Copyright (C) 2000 Erich Frühstück
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

#include "DocBuf.h"
#include <EFEU/strbuf.h>

void DocBuf_title (DocBuf *doc, StrBuf *base, const char *name)
{
	if	(sb_size(base) == 0)
		return;

	const char *title = doc->var[VAR_TITLE];
	StrBuf *buf = sb_acquire();
	DocBuf_copy(doc, base, buf, name);
	size_t n = sb_size(buf);

	if	(n && doc->var[VAR_TITLE] == title)
	{
		int newline = 0;

		/*	Reduktion auf ersten Absatz
		*/

		for (buf->pos = 0; buf->pos < n; buf->pos++)
		{
			if	(buf->data[buf->pos] != '\n')
			{
				newline = 0;
			}
			else if	(newline)
			{
				sb_sync(buf);
				break;
			}
			else
			{
				newline = 1;
			}
		}
			
		memfree(doc->var[VAR_TITLE]);
		doc->var[VAR_TITLE] = sb_strcpy(buf);
	}

	sb_release(buf);
}
