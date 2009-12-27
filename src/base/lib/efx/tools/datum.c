/*	Konvertierung von Datumsangaben in Tageszähler zur Basis 1900
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/datum.h>

OldDatum_t int2Dat(unsigned d)
{
	static OldDatum_t x;

	x.jahr = (d + .1) / 365.25;
	d -= 365 * x.jahr + (x.jahr + 3) / 4;

	if	(d < 59 || (x.jahr % 4 == 0 && d == 59))
	{
		x.monat = 1 + d / 31;
		x.tag = d + 32 - 31 * x.monat;
	}
	else
	{
		if	(x.jahr % 4 == 0)	d--;

		x.monat = (int) ((d + 33.3) / 30.6);
		x.tag = (int) (d + 34.3 - 30.6 * x.monat);
	}

	x.jahr += 1900;
	return x;
}


unsigned Dat2int(OldDatum_t dat)
{
	unsigned x;

	x = 365 * (dat.jahr - 1900) + 31 * dat.monat + dat.tag - 32;

	if	(dat.monat > 2)
	{
		x -= (short) (.4 * dat.monat + 2.3);
		x += (dat.jahr + 4 - 1900) / 4;
	}
	else	x += (dat.jahr + 3 - 1900) / 4;

	return x;
}


char *MoName[] = {
	NULL, "Jänner", "Februar", "März", "April", "Mai", "Juni", "Juli",
	"August", "September", "Oktober", "November", "Dezember",
};

char *WoName[] = {
	"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag",
};


#define	MONAT(dat)	MoName[(dat).monat % 13]
#define	WTAG(dat)	WoName[Dat2int(dat) % 7]

int Dat_WTAG (OldDatum_t dat)
{
	return (Dat2int(dat) % 7);
}

int Dat_TDJ (OldDatum_t dat)
{
	return (Dat2int(dat) - Dat2int(Datum(1, 1, dat.jahr)) + 1);
}

int Dat_WDJ (OldDatum_t dat, int base)
{
	int t0;
	int t1;

	t1 = Dat2int(dat);
	dat.tag = dat.monat = 1;
	t0 = Dat2int(dat);
	return (t1 - t0 + 1 + (t0 + 6 - base) % 7) / 7;
}

static int print_key(io_t *io, int key, OldDatum_t dat)
{
	switch (key)
	{
	case 'a':	return io_printf(io, "%.2s", WTAG(dat));
	case 'A':	return io_puts(WTAG(dat), io);
	case 'h':
	case 'b':	return io_printf(io, "%.3s", MONAT(dat));
	case 'B':	return io_puts(MONAT(dat), io);
	case 'd':	return io_printf(io, "%02d", dat.tag);
	case 'D':	return io_printf(io, "%02d/%02d/%02d", dat.monat, dat.tag, dat.jahr);
	case 'e':	return io_printf(io, "%2d", dat.tag);
	case 'j':	return io_printf(io, "%03d", Dat_TDJ(dat));
	case 'm':	return io_printf(io, "%02d", dat.monat);
	case 'n':	return io_nputc('\n', io, 1);
	case 't':	return io_nputc('\t', io, 1);
	case 'U':	return io_printf(io, "%02d", Dat_WDJ(dat, 0));
	case 'W':	return io_printf(io, "%02d", Dat_WDJ(dat, 1));
	case 'w':	return io_printf(io, "%d", Dat2int(dat) % 7);
	case 'y':	return io_printf(io, "%02d", dat.jahr % 100);
	case 'Y':	return io_printf(io, "%4d", dat.jahr);
	default:	return io_nputc(key, io, 1);
	}
}

int PrintDatum(io_t *io, const char *fmt, OldDatum_t dat)
{
	int n;

	if	(fmt == NULL)
	{
		return io_printf(io, "%2d.%2d.%02d", dat.tag, dat.monat, dat.jahr % 100);
	}

	for (n = 0; *fmt != 0; fmt++)
	{
		if	(*fmt == '%')
		{
			fmt++;

			if	(*fmt == 0)	break;

			n += print_key(io, *fmt, dat);
		}
		else	n += io_nputc(*fmt, io, 1);
	}

	return n;
}


char *KonvDatum(const char *fmt, int tz)
{
	strbuf_t *sb;
	io_t *io;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	PrintDatum(io, fmt, int2Dat(tz));
	io_close(io);
	return sb2str(sb);
}
