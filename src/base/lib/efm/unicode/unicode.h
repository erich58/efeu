/*
:*:unicode tools
:de:Unicode-Hilfsfunktionen

$Header	<EFEU/$1>

$Copyright (C) 2009 Erich Frühstück
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

#ifndef	EFEU_unicode_h
#define	EFEU_unicode_h	1

#include <EFEU/stdint.h>

int32_t latin9_to_ucs (int c);
int ucs_to_latin9 (int32_t c);

int32_t pgetucs (char **p, size_t lim);
int nputucs (int32_t c, unsigned char *buf, size_t len);

int ucscopy_utf8 (char *tg, size_t len, const char *src);
int ucscopy_latin9 (char *tg, size_t len, const char *src);

#endif	/* EFEU/unicode.h */
