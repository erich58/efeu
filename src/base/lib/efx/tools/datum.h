/*	Datumskonvertierungen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_DATUM_H
#define	EFEU_DATUM_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>

typedef struct {
#if	REVBYTEORDER
	ushort_t jahr;
	uchar_t monat;
	uchar_t tag;
#else
	uchar_t tag;
	uchar_t monat;
	ushort_t jahr;
#endif
} OldDatum_t;

unsigned Dat2int (OldDatum_t dat);
OldDatum_t int2Dat (unsigned dat);
OldDatum_t Datum (int tag, int monat, int jahr);

int Dat_WTAG (OldDatum_t dat);
int Dat_TDJ (OldDatum_t dat);
int Dat_WDJ (OldDatum_t dat, int base);

int PrintDatum (io_t *io, const char *fmt, OldDatum_t dat);

OldDatum_t str2Dat (const char *str);
char *Dat2str (OldDatum_t dat);
int TestDat (OldDatum_t dat);

char *KonvDatum(const char *fmt, int datum);

extern char *MoName[13];
extern char *WoName[7];

#endif	/* EFEU_DATUM_H */
