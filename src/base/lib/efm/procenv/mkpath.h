/*
:*:	tools for path construction
:de:	Hilfsfunktionen zur Pfadkonstruktion

$Header	<EFEU/$1>

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

#ifndef	EFEU_mkpath_h
#define	EFEU_mkpath_h	1

#include <EFEU/procenv.h>
#include <EFEU/strbuf.h>

int mkpath_bin (StrBuf *sb, const char *dir, ...);
void mkpath_base (StrBuf *sb, ...);
void mkpath_add (StrBuf *sb, const char *part);
char *mkpath (const char *dir, const char *env, ...);

#endif	/* EFEU/mkpath.h */
