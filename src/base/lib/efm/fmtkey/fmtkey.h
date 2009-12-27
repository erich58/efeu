/*
Formatierungsschl�ssel

$Header	<EFEU/$1>

$Copyright (C) 1994, 2007 Erich Fr�hst�ck
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

#ifndef	EFEU_fmtkey_h
#define	EFEU_fmtkey_h	1

#include <EFEU/io.h>
#include <EFEU/strbuf.h>
#include <EFEU/stdint.h>

typedef struct {
	int mode;	/* Formatierungsmodus */
	int flags;	/* Steuerflags */
	int size;	/* Gr��enangabe */
	int width;	/* Feldbreite */
	int prec;	/* Pr�zession */
	char *list;	/* Zeichenliste */
} FmtKey;

int io_fmtkey (IO *io, FmtKey *key);
int fmtkey (const char *fmt, FmtKey *key);

#define	FMT_BLANK	0x1	/* Blank bei positiven Werten */
#define	FMT_SIGN	0x2	/* Vorzeichen immer ausgeben */
#define	FMT_ALTMODE	0x4	/* Alternative Form */
#define	FMT_ZEROPAD	0x8	/* Auff�llen mit Leerzeichen */
#define	FMT_RIGHT	0x10	/* Rechtsb�ndige Ausrichtung */
#define	FMT_NEED_WIDTH	0x20	/* Feldbreite mu� abgefragt werden */
#define	FMT_NEED_PREC	0x40	/* Genauigkeit mu� abgefragt werden */
#define	FMT_NOPREC	0x80	/* Keine Genauigkeit angegeben */
#define	FMT_NEGPREC	0x100	/* negative Genauigkeit */
#define	FMT_GROUP	0x200	/* Grupperiungsflag */

#define	FMTKEY_BYTE	1	/* 1 Byte Datenwert */
#define	FMTKEY_SHORT	2	/* Kurzer Datenwert */
#define	FMTKEY_LONG	3	/* Langer Datenwert */
#define	FMTKEY_XLONG	4	/* Sehr Langer Datenwert */
#define	FMTKEY_SIZE	5	/* size_t kompatibler Datenwert */
#define	FMTKEY_PDIFF	6	/* ptrdiff_t kompatibler Datenwert */
#define	FMTKEY_IMAX	7	/* intmax_t kompatibler Datenwert */

void ftool_addsig (StrBuf *buf, int sig, int flags);
int ftool_ioalign (IO *io, StrBuf *sb, const FmtKey *key);

int fmt_bool (IO *io, const FmtKey *key, int val);
int fmt_char (IO *io, const FmtKey *key, int val);
int fmt_ucs (IO *io, const FmtKey *key, int32_t val);
int fmt_str (IO *io, const FmtKey *key, const char *val);
int fmt_long (IO *io, const FmtKey *key, long val);
int fmt_double (IO *io, const FmtKey *key, double val);
int fmt_intmax (IO *io, const FmtKey *key, intmax_t val);
int fmt_uintmax (IO *io, const FmtKey *key, uintmax_t val);

#endif	/* EFEU/fmtkey.h */
