/*
:*:	tools for path construction
:de:	Hilfsfunktionen zur Pfadkonstruktion

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

#include <EFEU/mkpath.h>

static int add_bin (StrBuf *sb, const char *name, size_t size, va_list list)
{
	const char *ext;

	if	(strncmp(name, "bin", 3) == 0)
	{
		size = 0;
	}
	else if	(size < 4)
	{
		return 0;
	}
	else if	(size >= 4 && strncmp(name + size - 4, "/bin", 4) == 0)
	{
		size -= 3;
	}
	else	return 0;

	while ((ext = va_arg(list, const char *)))
	{
		if	(sb->pos)	sb_putc(':', sb);

		sb_nputs(name, size, sb);
		sb_puts(ext, sb);
	}

	return 1;
}

static int mkpath_vbase (StrBuf *sb, va_list list)
{
	const char *endptr;
	const char *path;
	int n;

	path = getenv("PATH");

	if	(!path)	return 0;

	for (n = 0; (endptr = strchr(path, ':')); path = endptr + 1)
	{
		va_list tmp;
		va_copy(tmp, list);
		n += add_bin(sb, path, endptr - path, tmp);
		va_end(tmp);
	}

	return n + add_bin(sb, path, strlen(path), list);
}

void mkpath_base (StrBuf *sb, ...)
{
	va_list list;
	va_start(list, sb);
	mkpath_vbase(sb, list);
	va_end(list);
}

int mkpath_bin (StrBuf *sb, const char *dir, ...)
{
	va_list list;
	int n;

	va_start(list, dir);
	n = add_bin(sb, dir, strlen(dir), list);
	va_end(list);
	return n;
}

void mkpath_add (StrBuf *sb, const char *dir)
{
	if	(dir)
	{
		if	(sb->pos)	sb_putc(':', sb);

		sb_puts(dir, sb);
	}
}

static char *vmkpath (const char *dir, const char *env, va_list list)
{
	StrBuf *sb = sb_create(0);

	if	(dir)
	{
		va_list tmp;

		va_copy(tmp, list);

		if	(!add_bin(sb, dir, strlen(dir), tmp))
			mkpath_add(sb, dir);

		va_end(tmp);
	}

	if	(env)
		mkpath_add(sb, getenv(env));
		
	mkpath_vbase(sb, list);
	return sb2str(sb);
}

char *mkpath (const char *dir, const char *env, ...)
{
	va_list list;
	char *p;

	va_start(list, env);
	p = vmkpath(dir, env, list);
	va_end(list);
	return p;
}
