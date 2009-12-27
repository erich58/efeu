/*
Formatierungsschlüssel

$Header	<EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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
	int width;	/* Feldbreite */
	int prec;	/* Präzession */
	char *list;	/* Zeichenliste */
} FmtKey;

int io_fmtkey (IO *io, FmtKey *key);
int fmtkey (const char *fmt, FmtKey *key);

#define	FMT_BLANK	0x1	/* Blank bei positiven Werten */
#define	FMT_SIGN	0x2	/* Vorzeichen immer ausgeben */
#define	FMT_ALTMODE	0x4	/* Alternative Form */
#define	FMT_ZEROPAD	0x8	/* Auffüllen mit Leerzeichen */
#define	FMT_RIGHT	0x10	/* Rechtsbündige Ausrichtung */
#define	FMT_NEED_WIDTH	0x20	/* Feldbreite muß abgefragt werden */
#define	FMT_NEED_PREC	0x40	/* Genauigkeit muß abgefragt werden */
#define	FMT_NOPREC	0x80	/* Keine Genauigkeit angegeben */
#define	FMT_NEGPREC	0x100	/* negative Genauigkeit */
#define	FMT_BYTE	0x200	/* 1 Byte Datenwert */
#define	FMT_SHORT	0x400	/* Kurzer Datenwert */
#define	FMT_LONG	0x800	/* Langer Datenwert */
#define	FMT_XLONG	0x1000	/* Sehr Langer Datenwert */

void ftool_addsig (StrBuf *buf, int sig, int flags);
int ftool_ioalign (IO *io, StrBuf *sb, const FmtKey *key);

int fmt_bool (IO *io, const FmtKey *key, int val);
int fmt_char (IO *io, const FmtKey *key, int val);
int fmt_ucs (IO *io, const FmtKey *key, int32_t val);
int fmt_str (IO *io, const FmtKey *key, const char *val);
int fmt_long (IO *io, const FmtKey *key, long val);
int fmt_double (IO *io, const FmtKey *key, double val);
int fmt_int64 (IO *io, const FmtKey *key, int64_t val);
int fmt_uint64 (IO *io, const FmtKey *key, uint64_t val);

#endif	/* EFEU/fmtkey.h */
