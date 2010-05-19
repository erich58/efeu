/*
Formatierungsschlüssel

$Header	<EFEU/$1>

$Copyright (C) 1994, 2007 Erich Frühstück
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

#include <EFEU/strbuf.h>
#include <EFEU/stdint.h>
#include <EFEU/mstring.h>

typedef struct {
	StrBuf buf;	/* Zeichenbuffer */
	int mode;	/* Formatierungsmodus */
	int flags;	/* Steuerflags */
	int size;	/* Größenangabe */
	int width;	/* Feldbreite */
	int prec;	/* Präzession */
} FmtKey;

#define	FKEY_DATA()	{ SB_DATA(30), 0, 0, 0, 0, 0 }
#define	FKEY_DECL(name)	FmtKey name = FKEY_DATA()

#define	FMT_BLANK	0x1	/* Blank bei positiven Werten */
#define	FMT_SIGN	0x2	/* Vorzeichen immer ausgeben */
#define	FMT_ALTMODE	0x4	/* Alternative Form */
#define	FMT_ZEROPAD	0x8	/* Auffüllen mit 0 */
#define	FMT_RIGHT	0x10	/* Rechtsbündige Ausrichtung */
#define	FMT_NEED_WIDTH	0x20	/* Feldbreite muß abgefragt werden */
#define	FMT_NEED_PREC	0x40	/* Genauigkeit muß abgefragt werden */
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

FmtKey *fmtkey (FmtKey *buf, int32_t (*get) (void *), void *);
int32_t fmtkey_pgetc (void *data);
int32_t fmtkey_pgetucs (void *data);
int32_t pgetucs (char **p, size_t lim);

int ftool_addsig (StrBuf *buf, int sig, int flags);
int ftool_complete (StrBuf *sb, const FmtKey *key, int pos, int nchar);
int ftool_align (StrBuf *sb, const FmtKey *key, int pos, int nchar);

int fmt_bool (StrBuf *sb, const FmtKey *key, int val);
int fmt_char (StrBuf *sb, const FmtKey *key, int val);
int fmt_ucs (StrBuf *sb, const FmtKey *key, int32_t val);
int fmt_str (StrBuf *sb, const FmtKey *key, const char *val);
int fmt_mbstr (StrBuf *sb, const FmtKey *key, const char *val);
int fmt_long (StrBuf *sb, const FmtKey *key, long val);
int fmt_double (StrBuf *sb, const FmtKey *key, double val);
int fmt_intmax (StrBuf *sb, const FmtKey *key, intmax_t val);
int fmt_uintmax (StrBuf *sb, const FmtKey *key, uintmax_t val);

#endif	/* EFEU/fmtkey.h */
