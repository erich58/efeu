/*	Bestimmung eines Datumsbereichs
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/TimeRange.h>
#include <EFEU/calendar.h>
#include <EFEU/ioscan.h>
#include <ctype.h>

#define	DELIM	"%s,"

#define	JLIM	70	/* Limit für Jahrhundertwechsel */

char *TimeRange_fmt = NULL;

/*
Die Funktion |$1| synchronisiert die Komponenten eines Zeitbereichs
mit den Ober- und Untergrenzen.
*/

void TimeRange_sync (TimeRange *rng)
{
	rng->tage = rng->og - rng->ug + 1;
	rng->dat = (rng->og + rng->ug) / 2;
	rng->jahr = Calendar(rng->dat, NULL)->year;
}

/*	Datumsbereich
*/

#define	DAT_SEC		1	/* Sekundenangabe */
#define	DAT_MIN		2	/* Minutenangabe */
#define	DAT_HOUR	3	/* Stundenangabe */
#define	DAT_TAG		4	/* Tagesangabe */
#define	DAT_MONAT	5	/* Monatsangabe */
#define	DAT_QUART	6	/* Quartalsangabe */
#define	DAT_JAHR	7	/* Jahresangabe */

typedef struct {
	unsigned type : 8;	/* Datumstype */
	unsigned val : 24;	/* Datumswert */
	unsigned sec : 32;	/* Sekunden */
} DAT;

/*	Jahresangabe standardisieren
*/

static int sync_jahr(int jahr)
{
	if	(jahr < JLIM)	return 2000 + jahr;
	if	(jahr < 100)	return 1900 + jahr;

	return jahr;
}

/*	Datumswert bestimmen
*/

static int next_val(IO *io, int *vptr)
{
	int c;
	void *ptr;

	c = io_getc(io);
	io_ungetc(c, io);

	if	(isdigit(c))
	{
		io_scan(io, SCAN_INT, &ptr);
		*vptr = *((int *) ptr);
		return 1;
	}

	return 0;
}

static int sub_val(IO *io, int x, int *ptr)
{
	int c = io_getc(io);

	if	(c == x)
	{
		if	(next_val(io, ptr))
			return 1;
	}

	io_ungetc(c, io);
	return 0;
}


/*	Datumseintrag
*/

static DAT *get_dat(IO *io, DAT *ptr, char *delim)
{
	static DAT buf;
	int a, b, c;

	if	(ptr == NULL)	ptr = &buf;

	ptr->type = 0;
	ptr->val = 0;
	ptr->sec = 0;

	while ((c = io_eat(io, delim)) == '#')
	{
		do	c = io_getc(io);
		while 	(c != EOF && c != '\n');
	}

	if	(c == EOF)	return NULL;

	if	(!next_val(io, &a))
	{
		io_error(io, "[mdmat:305]", "c", io_getc(io));
		return NULL;
	}

	if	(sub_val(io, '-', &b))
	{
		if	(sub_val(io, '-', &c))
		{
			ptr->type = DAT_TAG;
			ptr->val = CalendarIndex(c, b, sync_jahr(a));
		}
		else
		{
			ptr->type = DAT_MONAT;
			ptr->val = sync_jahr(a) * 12 + b - 1; 
		}
	}
	else if	(sub_val(io, '.', &b))
	{
		if	(sub_val(io, '.', &c))
		{
			ptr->type = DAT_TAG;
			ptr->val = CalendarIndex(a, b, sync_jahr(c));
		}
		else
		{
			ptr->type = DAT_MONAT;
			ptr->val = sync_jahr(b) * 12 + a - 1; 
		}
	}
	else if	(sub_val(io, ':', &b))
	{
		ptr->type = DAT_MIN;
		ptr->sec = (a * 60 + b) * 60;
		
		if	(sub_val(io, ':', &c))
		{
			ptr->type = DAT_SEC;
			ptr->sec += c;
		}
	}
	else if	(sub_val(io, '/', &b))
	{
		ptr->type = DAT_QUART;
		ptr->val = sync_jahr(a) * 4 + b - 1; 
	}
	else
	{
		ptr->type = DAT_JAHR;
		ptr->val = sync_jahr(a);
	}

	if	(ptr->type == DAT_TAG && sub_val(io, ' ', &a))
	{
		ptr->type = DAT_HOUR;
		ptr->sec = a * 60 * 60;

		if	(sub_val(io, ':', &a))
		{
			ptr->type = DAT_MIN;
			ptr->sec += a * 60;

			if	(sub_val(io, ':', &a))
			{
				ptr->type = DAT_SEC;
				ptr->sec += a;
			}
		}
	}

	return ptr;
}


/*	Standarddatumsangabe
*/

static char *dat_label (DAT *dat)
{
	static char buf[16];
	char *p;

	switch (dat->type)
	{
	case DAT_MONAT:
		sprintf(buf, "%02d.%d", dat->val % 12 + 1, dat->val / 12);
		break;
	case DAT_QUART:
		sprintf(buf, "%d:%d", dat->val / 4, dat->val % 4 + 1);
		break;
	case DAT_JAHR:
		sprintf(buf, "%d", dat->val);
		break;
	default:
		p = Calendar2str(TimeRange_fmt, dat->val);
		sprintf(buf, "%s", p);
		memfree(p);
		break;
	}

	return buf;
}

static unsigned dat_lim (int type, int val)
{
	switch (type)
	{
	case DAT_MONAT:	return CalendarIndex(1, val % 12 + 1, val / 12);
	case DAT_QUART:	return CalendarIndex(1, 3 * (val % 4) + 1, val / 4);
	case DAT_JAHR:	return CalendarIndex(1, 1, val);
	default:	return val;
	}
}

static unsigned sec_lim (int type, int val)
{
	switch (type)
	{
	case DAT_SEC:	return val;
	case DAT_MIN:	return val - val % 60;
	case DAT_HOUR:	return val - val % 3600;
	default:	return 0;
	}
}

/*	Neuer Datumsbereich
*/

static void reduce_type (DAT *dat, int flag)
{
	switch (dat->type)
	{
	case DAT_JAHR:
		dat->val = dat->val * 4 + 3 * flag;
		dat->type = DAT_QUART;
		break;
	case DAT_QUART:
		dat->val = dat->val * 3 +  2 * flag;
		dat->type = DAT_MONAT;
		break;
	case DAT_MONAT:
		dat->val = dat_lim(dat->type, dat->val + flag) - flag;
		dat->type = DAT_TAG;
		break;
	default:
		break;
	}
}

static void sync_dat(DAT *a, DAT *b)
{
	while (a->type > b->type)
		reduce_type(a, 0);

	while (b->type > a->type)
		reduce_type(b, 1);
}

static TimeRange *new_range(VecBuf *buf, DAT *a, DAT *b, int flag, char *fmt)
{
	TimeRange *range = vb_next(buf);

	range->ug = dat_lim(a->type, a->val);
	range->og = dat_lim(b->type, b->val + 1) - 1;
	range->ug_sec = sec_lim(a->type, a->val);
	range->og_sec = sec_lim(b->type, b->val + 1) - 1;
	TimeRange_sync(range);

	if	(fmt)
	{
		range->label = Calendar2str(fmt, range->dat);
	}
	else if	(flag == 1)
	{
		char *p = mstrcpy(dat_label(a));
		range->label = msprintf("%s-%s", p, dat_label(b));
		memfree(p);
	}
	else if	(flag == 2)
	{
		range->label = mstrcpy(dat_label(b));
	}
	else	range->label = mstrcpy(dat_label(a));

	return range;
}

static int time_cmp (const void *a_ptr, const void *b_ptr)
{
	const TimeRange *a = a_ptr;
	const TimeRange *b = b_ptr;

	if	(a->ug < b->ug)	return -1;
	if	(a->ug > b->ug)	return 1;
	if	(a->og > b->og)	return -1;
	if	(a->og < b->og)	return 1;

	return	0;
}

int IOTimeRange (VecBuf *buf, IO *io)
{
	DAT a, b;
	TimeRange *range;
	char *p;
	int c;
	int mintype;
	unsigned last;

	mintype = DAT_JAHR;
	last = 0;

	while (get_dat(io, &a, DELIM) != NULL)
	{
		p = io_getarg(io, '[', ']');
		c = io_eat(io, DELIM);

		if	(c == '-')
		{
			c = io_getc(io);

			if	(!get_dat(io, &b, DELIM))	break;
		
			sync_dat(&a, &b);
			memfree(p);
			p = io_getarg(io, '[', ']');

			if	(a.val <= b.val)
			{
				range = new_range(buf, &a, &b, 1, p);
				p = NULL;
			}

			last = 0;
		}
		else if	(c == '.')
		{
			io_getc(io);
			c = io_getc(io);

			if	(c != '.')
			{
				io_error(io, "[mdmat:305]", "c", c);
				break;
			}

			if	(!get_dat(io, &b, DELIM))	break;

			memfree(p);
			p = io_getarg(io, '[', ']');
		
			for (sync_dat(&a, &b); a.val <= b.val; a.val++)
				range = new_range(buf, &a, &a, 0, p);

			last = 0;
		}
		else
		{
			if	(a.type > DAT_TAG)
			{
				range = new_range(buf, &a, &a, 0, p);
				p = NULL;
				last = 0;
			}
			else if	(last)
			{
				b = a;
				a.val = last + 1;
				range = new_range(buf, &a, &b, 2, p);
				p = NULL;
				last = (c != ';' && c != EOF) ? b.val : 0;
			}
			else if	(c == ';' || c == EOF)
			{
				range = new_range(buf, &a, &a, 0, p);
				p = NULL;
				last = 0;
			}
			else	last = a.val;

			if	(c == ';')
				io_getc(io);
		}

		memfree(p);

		if	(mintype > a.type)	mintype = a.type;
	}

	vb_qsort(buf, time_cmp);

	switch (mintype)
	{
	case DAT_TAG:	return 't';
	case DAT_MONAT:	return 'm';
	case DAT_QUART:	return 'q';
	case DAT_JAHR:	return 'j';
	case DAT_SEC:	return 'S';
	case DAT_MIN:	return 'M';
	case DAT_HOUR:	return 'H';
	default:	return 0;
	}
}

int strTimeRange (VecBuf *buf, const char *def)
{
	IO *io;
	int type;

	if	(def == NULL)
	{
		return 0;
	}
	else if	(def[0] == '@')
	{
		io = io_lnum(io_fileopen(def + 1, "rz"));
	}
	else	io = io_cstr(def);

	type = IOTimeRange(buf, io);
	io_close(io);
	return type;
}

static int put_sec (IO *io, unsigned sec)
{
	return io_printf(io, " %02d:%02d:%02d", sec / 3600,
		(sec / 60) % 60, sec % 60);
}

int PrintTimeRange (IO *io, TimeRange *rng, size_t n)
{
	int i;

	for (i = 0; n-- > 0; rng++)
	{
		i += io_xprintf(io, "%-12s\t", rng->label);

		if	(rng->tage)
			i += PrintCalendar(io, "%d.%m.%Y", rng->ug);

		if	(rng->sec)
			i += put_sec(io, rng->ug_sec);

		i += io_puts(" - ", io);

		if	(rng->sec)
			i += put_sec(io, rng->og_sec);

		if	(rng->tage)
			i += PrintCalendar(io, "%d.%m.%Y", rng->og);

		i += PrintCalendar(io, "%t%d.%m.%Y ", rng->dat);

		if	(rng->tage)
			i += io_xprintf(io, "%4d", rng->tage);

		if	(rng->sec)
		{
			int h = rng->sec / 3600;
			int m = (rng->sec / 60) % 60;
			int s = rng->sec % 60;

			if	(rng->tage)
				i += io_puts("+", io);

			if	(h)	i += io_printf(io, "%dh", h);
			if	(m)	i += io_printf(io, "%dm", m);
			if	(s)	i += io_printf(io, "%ds", s);
		}

		i += io_puts("\n", io);
	}

	return i;
}
