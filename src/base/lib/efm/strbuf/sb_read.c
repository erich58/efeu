/*
:*:read pending data from file descriptor
:de:Anstehende Daten in Stringbuffer einlesen

$Copyright (C) 2007 Erich Frühstück
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

#include <EFEU/strbuf.h>
#include <EFEU/Debug.h>
#include <errno.h>
#include <sys/ioctl.h>

static StrBuf read_buf = SB_DATA(0);

char *sb_read (int fd, StrBuf *sb)
{
	int nread = 0;

	if	(!sb)
		sb = &read_buf;

	sb_begin(sb);
	sb_sync(sb);

	do
	{
		ssize_t n;

		if	(!sb->nfree)
			sb_expand(sb);

		if ((n = read(fd, sb->data + sb->pos, sb->nfree)) < 0)
		{
			dbg_error(NULL, "$!: read: $1\n", "s",
				strerror(errno));
			break;
		}

		sb->pos += n;
		sb_sync(sb);

		if	(ioctl(fd, FIONREAD, &nread))
		{
			dbg_error(NULL, "$!: ioctl: $1\n", "s",
				strerror(errno));
			break;
		}
	}
	while (nread);

	if	(sb->pos)
	{
		sb_putc(0, sb);
		return (char *) sb->data;
	}
	else	return NULL;
}

/*
$SeeAlso
\mref{strbuf(3)},
\mref{strbuf(7)}.
*/
