/*
Datenwerte formatiern

$Copyright (C) 1994, 2008 Erich Frühstück
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

#define	_ISOC99_SOURCE

#include <EFEU/strbuf.h>
#include <stdarg.h>

/*
Die Funktion |$1| erlaubt formatierte Ausgaben in den Stringbuffer <sb>.
Es wird ein implizites |sb_sync| durchgeführt. Der Stringbuffer ist
anschließend nullterminiert. Ein Nullpointer als Formatstring ist zulässig.
*/

int sb_printf (StrBuf *sb, const char *fmt, ...)
{
	va_list args;
	int n;

	va_start(args, fmt);
	n = sb_vprintf(sb, fmt, args);
	va_end(args);
	return n;
}

/*
Die Funktion |$1| hat die selbe Funktionalität wie |sb_printf|, nur wird
ein Argument vom Typ |va_list| anstelle einer variablen Argumentliste
verwendet.
*/

int sb_vprintf (StrBuf *sb, const char *fmt, va_list list)
{
	int n;
	va_list args;
#if	! HAS_SNPRINTF
	FILE *file;
	char *p;
#endif

	if	(!sb)	return 0;

	sb_sync(sb);

#if	HAS_SNPRINTF
	if	(!sb->nfree)
		sb_expand(sb, 1);

	if	(!fmt)
	{
		sb->data[sb->pos] = 0;
		return 0;
	}

	va_copy(args, list);

	while ((n = vsnprintf(sb_ptr(sb), sb->nfree, fmt, args)) >= sb->nfree)
	{
		sb_expand(sb, n + 1);
		va_end(args);
		va_copy(args, list);
	}

#else
	if	(!(file = tmpfile()))
		return 0;

	n = vfprintf(file, fmt, list);
	p = sb_expand(sb, n + 1);
	rewind(file);
	n = fread(p, 1, n, file);
	p[n] = 0;
	fclose(file);
#endif
	va_end(args);
	sb->pos += n;
	sb->nfree -= n;
	return n;
}
