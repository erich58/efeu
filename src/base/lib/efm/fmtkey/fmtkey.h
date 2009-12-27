/*
Formatierungsschl�ssel

$Header	<EFEU/$1>

$Copyright (C) 1994 Erich Fr�hst�ck
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

#ifndef	_EFEU_fmtkey_h
#define	_EFEU_fmtkey_h	1

#include <EFEU/io.h>
#include <EFEU/strbuf.h>

typedef struct {
	int mode;	/* Formatierungsmodus */
	int flags;	/* Steuerflags */
	int width;	/* Feldbreite */
	int prec;	/* Pr�zession */
	char *list;	/* Zeichenliste */
} fmtkey_t;

int fmtkey (const char *str, fmtkey_t *key);

#define	FMT_BLANK	0x1	/* Blank bei positiven Werten */
#define	FMT_SIGN	0x2	/* Vorzeichen immer ausgeben */
#define	FMT_ALTMODE	0x4	/* Alternative Form */
#define	FMT_ZEROPAD	0x8	/* Auff�llen mit Leerzeichen */
#define	FMT_RIGHT	0x10	/* Rechtsb�ndige Ausrichtung */
#define	FMT_NEED_WIDTH	0x20	/* Feldbreite mu� abgefragt werden */
#define	FMT_NEED_PREC	0x40	/* Genauigkeit mu� abgefragt werden */
#define	FMT_NOPREC	0x80	/* Keine Genauigkeit angegeben */
#define	FMT_NEGPREC	0x100	/* negative Genauigkeit */
#define	FMT_SHORT	0x200	/* Kurzer Datenwert */
#define	FMT_LONG	0x400	/* Langer Datenwert */
#define	FMT_XLONG	0x800	/* Sehr Langer Datenwert */

void ftool_addsig (strbuf_t *buf, int sig, int flags);
int ftool_ioalign (io_t *io, strbuf_t *sb, const fmtkey_t *key);

int fmt_bool (io_t *io, const fmtkey_t *key, int val);
int fmt_char (io_t *io, const fmtkey_t *key, int val);
int fmt_str (io_t *io, const fmtkey_t *key, const char *val);
int fmt_long (io_t *io, const fmtkey_t *key, long val);
int fmt_double (io_t *io, const fmtkey_t *key, double val);

#endif	/* EFEU/fmtkey.h */
