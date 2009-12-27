/*	Formatierungsschl�ssel
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_FMTKEY_H
#define	EFEU_FMTKEY_H	1

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
