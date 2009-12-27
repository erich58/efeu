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

char *io_getname (IO *io);
int io_testkey (IO *io, const char *str);
int io_scan (IO *io, unsigned flags, void **ptr);
int io_valscan (IO *io, unsigned flags, void **ptr);

#define	SCAN_BINVAL	0x1	/* Binärwert */
#define	SCAN_OCTVAL	0x2	/* Oktalwert */
#define	SCAN_HEXVAL	0x4	/* Hexadezimalwert */
#define	SCAN_UNSIGNED	0x8	/* Vorzeichenfreier Wert */
#define	SCAN_MODEMASK	0xf	/* Maske für Modifikationsflags */

#define	SCAN_INT	0x10	/* Normaler Ganzzahlwert */
#define	SCAN_LONG	0x20	/* Langer Ganzzahlwert */
#define	SCAN_INTEGER	0x3f	/* Beliebiger Ganzzahlwert */
#define	SCAN_DOUBLE	0x40	/* Gleitkommazahl */
#define	SCAN_VALMASK	0x70	/* Maske für Zahlenwerte */
#define	SCAN_ANYVAL	0x7f	/* Beliebiger Zahlenwert */

#define	SCAN_CHAR	0x100	/* Zeichendefinition (einfache Anführung) */
#define	SCAN_STR	0x200	/* String (doppelte Anführung oder NULL) */
#define	SCAN_NULL	0x400	/* NULL - Key */
#define	SCAN_NAME	0x800	/* Name */

#define	SCAN_TYPEMASK	0xff0	/* Maske für Werttype */

#define	SCAN_ANYTYPE	0xfff	/* Beliebiger Type */

#endif	/* EFEU/ioscan.h */
