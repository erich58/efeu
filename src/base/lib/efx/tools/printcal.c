/*	Datumsformatierung
	(c) 1995 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 2.0
*/

#include <EFEU/calendar.h>
#include <EFEU/locale.h>

#define	LOCALE	(Locale.date ? Locale.date : &LCDate_DE)

#define	MONAT(dat)	LOCALE->month[(cal->month - 1) % 12]
#define	WTAG(idx)	LOCALE->weekday[cal->wday % 7]

#define	FMT_DATE	"%_d.%_m.%Y"
#define	FMT_TIME	"%_d.%_m.%Y %H:%M:%S"

static int print_cal(io_t *io, const char *fmt, Calendar_t *cal);

typedef int (*putval_t) (io_t *io, int w, int val);

static int pad_none(io_t *io, int width, int val)
{
	return io_printf(io, "%d", val);
}

static int pad_zero(io_t *io, int width, int val)
{
	return io_printf(io, "%0*d", width, val);
}

static int pad_space(io_t *io, int width, int val)
{
	return io_printf(io, "%*d", width, val);
}


static int sun_week(Calendar_t *cal)
{
	int x = (cal->yday - cal->wday - 1);
	return x <= 0 ?  0 : x / 7 + (x % 7 != 0);
}

static int mon_week(Calendar_t *cal)
{
	int x = cal->wday ? cal->wday - 1 : 6;
	x = cal->yday - x - 1;
	return x <= 0 ?  0 : x / 7 + (x % 7 != 0);
}


static int print_key_D (io_t *io, Calendar_t *cal, putval_t putval)
{
	int n = putval(io, 2, cal->month);
	n += io_nputc('/', io, 1);
	n += putval(io, 2, cal->day);
	n += io_nputc('/', io, 1);
	n += putval(io, 2, cal->year);
	return n;
}

static int print_key (io_t *io, Calendar_t *cal, int key, putval_t putval)
{
	switch (key)
	{
	case 'a':	return io_printf(io, "%2.2s", WTAG(cal));
	case 'A':	return io_puts(WTAG(cal), io);
	case 'h':
	case 'b':	return io_printf(io, "%3.3s", MONAT(cal));
	case 'B':	return io_puts(MONAT(cal), io);
	case 'd':	return putval(io, 2, cal->day);
	case 'D':	return print_key_D(io, cal, putval);
	case 'e':	return io_printf(io, "%2d", cal->day);
	case 'j':	return putval(io, 3, cal->yday);
	case 'm':	return putval(io, 2, cal->month);
	case 'U':	return putval(io, 2, sun_week(cal));
	case 'W':	return putval(io, 2, mon_week(cal));
	case 'w':	return io_printf(io, "%d", cal->wday);
	case 'y':	return putval(io, 2, cal->year % 100);
	case 'Y':	return putval(io, 4, cal->year);

	case 'H':	return putval(io, 2, cal->hour);
	case 'I':	return putval(io, 2, cal->hour % 12);
	case 'M':	return putval(io, 2, cal->min);
	case 'S':	return putval(io, 2, cal->sec);
	case 'p':	return io_puts(cal->hour >= 12 ? "PM" : "AM", io);
	case 'r':	return print_cal(io, "%I:%M:%S %p", cal);
	case 'T':	return print_cal(io, "%H:%M:%S", cal);
	case 'X':	return print_cal(io, "%H:%M:%S", cal);

	case 'n':	return io_nputc('\n', io, 1);
	case 't':	return io_nputc('\t', io, 1);
	default:	return io_nputc(key, io, 1);
	}
}


static int print_cal (io_t *io, const char *fmt, Calendar_t *cal)
{
	putval_t putval;
	int n;

	if	(fmt == NULL)
	{
		return io_printf(io, "%2d.%2d.%04d", cal->day, cal->month,
			cal->year);
	}

	for (n = 0; *fmt != 0; fmt++)
	{
		if	(*fmt == '%')
		{
			putval = pad_zero;

			for (;;)
			{
				fmt++;

				if	(*fmt == 0)	return n;
				else if	(*fmt == '-')	putval = pad_none;
				else if	(*fmt == '_')	putval = pad_space;
				else	break;
			}

			n += print_key(io, cal, *fmt, putval);
		}
		else	n += io_nputc(*fmt, io, 1);
	}

	return n;
}

int PrintCalendar(io_t *io, const char *fmt, int idx)
{
	return print_cal(io, fmt ? fmt : FMT_DATE, Calendar(idx, NULL));
}

int PrintTime(io_t *io, const char *fmt, Time_t time)
{
	return print_cal(io, fmt ? fmt : FMT_TIME, TimeCalendar(time, NULL));
}


/*	Konvertierung in Zeichenkette
*/

static char *cal2str (const char *fmt, Calendar_t *cal)
{
	strbuf_t *sb;
	io_t *io;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	print_cal(io, fmt, cal);
	io_close(io);
	return sb2str(sb);
}

char *Calendar2str (const char *fmt, int idx)
{
	return cal2str(fmt ? fmt : FMT_DATE, Calendar(idx, NULL));
}

char *Time2str (const char *fmt, Time_t time)
{
	return cal2str(fmt ? fmt : FMT_TIME, TimeCalendar(time, NULL));
}
