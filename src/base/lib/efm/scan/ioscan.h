/*
Datenwerte aus IO-Struktur lesen

$Header	<EFEU/$1>

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

#ifndef	EFEU_ioscan_h
#define	EFEU_ioscan_h	1

#include <EFEU/io.h>
#include <EFEU/stdint.h>

char *io_getname (IO *io);
int io_testkey (IO *io, const char *str);
int io_scan (IO *io, unsigned flags, void **ptr);
int io_valscan (IO *io, unsigned flags, void **ptr);

int64_t mstr2int64 (const char *str, char **ptr, int base);
uint64_t mstr2uint64 (const char *str, char **ptr, int base);

#define	SCAN_INT	0x1	/* Ganzzahlwert */
#define	SCAN_INT64	0x2	/* 64-Bit Ganzzahlwert */
#define	SCAN_FLOAT	0x4	/* Gleitkommazahl, einfache Genauigkeit */
#define	SCAN_DOUBLE	0x8	/* Gleitkommazahl, doppelte Genauigkeit */

#define	SCAN_CHAR	0x10	/* ASCII/Latin1 - Zeichendefinition */
#define	SCAN_UCS	0x20	/* UNICODE - Zeichendefinition */
#define	SCAN_STR	0x40	/* String (doppelte Anführung oder NULL) */
#define	SCAN_UTF	0x80	/* UTF8-String */
#define	SCAN_NAME	0x100	/* Name */

#define	SCAN_INTEGER	(SCAN_INT | SCAN_INT64)
#define	SCAN_NUMERIC	(SCAN_INTEGER | SCAN_FLOAT | SCAN_DOUBLE)
#define	SCAN_XCHAR	(SCAN_CHAR | SCAN_UCS)
#define	SCAN_TEXT	(SCAN_XCHAR | SCAN_STR | SCAN_UTF | SCAN_NAME)
#define	SCAN_TYPEMASK	(SCAN_NUMERIC | SCAN_TEXT)

#define	SCAN_BINVAL	0x400	/* Binärwert */
#define	SCAN_OCTVAL	0x800	/* Oktalwert */
#define	SCAN_HEXVAL	0x1000	/* Hexadezimalwert */

#define	SCAN_BASEMASK	(SCAN_BINVAL | SCAN_OCTVAL | SCAN_HEXVAL)

#define	SCAN_UNSIGNED	0x2000	/* Vorzeichenfreier Wert */
#define	SCAN_LOCALE	0x4000	/* Lokale berücksichtigen */

#define	SCAN_ANYVAL	(SCAN_NUMERIC | SCAN_UNSIGNED | SCAN_BASEMASK)
#define	SCAN_ANYTYPE	(SCAN_ANYVAL | SCAN_TEXT)

#endif	/* EFEU/ioscan.h */
