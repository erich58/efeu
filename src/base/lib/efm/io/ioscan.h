/*	Datenwerte aus IO-Struktur lesen
	(c) 1997 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_IOSCAN_H
#define	EFEU_IOSCAN_H	1

#include <EFEU/io.h>

int io_scan (io_t *io, unsigned flags, void **ptr);
int io_valscan (io_t *io, unsigned flags, void **ptr);

#define	SCAN_INT	0x01	/* Normaler Ganzzahlwert */
#define	SCAN_LONG	0x02	/* Langer Ganzzahlwert */
#define	SCAN_DOUBLE	0x04	/* Gleitkommazahl */

#define	SCAN_CHAR	0x10	/* Zeichendefinition (einfache Anf�hrung) */
#define	SCAN_STR	0x20	/* String (doppelte Anf�hrung oder NULL) */
#define	SCAN_NAME	0x40	/* Name */
#define	SCAN_NULL	0x80	/* NULL - Key */

#define	SCAN_BINVAL	0x0100	/* Bin�rwert */
#define	SCAN_OCTVAL	0x0200	/* Oktalwert */
#define	SCAN_HEXVAL	0x0400	/* Hexadezimalwert */
#define	SCAN_UNSIGNED	0x1000	/* Vorzeichenfreier Wert */

#define	SCAN_VALMASK	0x000f	/* Maske f�r Zahlenwerte */
#define	SCAN_TYPEMASK	0x00ff	/* Maske f�r Werttype */
#define	SCAN_BASEMASK	0x0f00	/* Maske f�r Ziffernbasis */
#define	SCAN_MODEMASK	0xff00	/* Maske f�r Modifikationsflags */

#define	SCAN_INTEGER	0x1f03	/* Beliebiger Ganzzahlwert */
#define	SCAN_ANYVAL	0x1f07	/* Beliebiger Zahlenwert */
#define	SCAN_ANYTYPE	0x1fff	/* Beliebiger Type */

#endif	/* EFEU/ioscan.h */
