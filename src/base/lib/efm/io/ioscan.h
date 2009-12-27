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

#ifndef	_EFEU_ioscan_h
#define	_EFEU_ioscan_h	1

#include <EFEU/io.h>

int io_scan (io_t *io, unsigned flags, void **ptr);
int io_valscan (io_t *io, unsigned flags, void **ptr);

#define	SCAN_INT	0x01	/* Normaler Ganzzahlwert */
#define	SCAN_LONG	0x02	/* Langer Ganzzahlwert */
#define	SCAN_DOUBLE	0x04	/* Gleitkommazahl */

#define	SCAN_CHAR	0x10	/* Zeichendefinition (einfache Anführung) */
#define	SCAN_STR	0x20	/* String (doppelte Anführung oder NULL) */
#define	SCAN_NAME	0x40	/* Name */
#define	SCAN_NULL	0x80	/* NULL - Key */

#define	SCAN_BINVAL	0x0100	/* Binärwert */
#define	SCAN_OCTVAL	0x0200	/* Oktalwert */
#define	SCAN_HEXVAL	0x0400	/* Hexadezimalwert */
#define	SCAN_UNSIGNED	0x1000	/* Vorzeichenfreier Wert */

#define	SCAN_VALMASK	0x000f	/* Maske für Zahlenwerte */
#define	SCAN_TYPEMASK	0x00ff	/* Maske für Werttype */
#define	SCAN_BASEMASK	0x0f00	/* Maske für Ziffernbasis */
#define	SCAN_MODEMASK	0xff00	/* Maske für Modifikationsflags */

#define	SCAN_INTEGER	0x1f03	/* Beliebiger Ganzzahlwert */
#define	SCAN_ANYVAL	0x1f07	/* Beliebiger Zahlenwert */
#define	SCAN_ANYTYPE	0x1fff	/* Beliebiger Type */

#endif	/* EFEU/ioscan.h */
