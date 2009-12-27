/*	Zeitreihenindex konvertieren
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <EFEU/calendar.h>
#include <Math/TimeSeries.h>

#define	MBASE(x)	((x) == TS_MONTH || (x) == TS_QUART || (x) == TS_YEAR)

#define	mittel(a,b)	(0.5 * ((a) + (b) + 1.))

static int dat2mon (unsigned dat)
{
	Calendar_t *cal = Calendar(dat, NULL);
	return cal->year * 12 + cal->month - 1;
}

TimeIndex_t TimeIndexKonv (TimeIndex_t idx, int type, int pos)
{
	if	(idx.type == type)	return idx;

	if	(MBASE(idx.type) && MBASE(type))
	{
		switch (idx.type)
		{
		case TS_YEAR:

			idx.value *= 12;

			if	(pos == 0)	idx.value += 6;
			else if	(pos > 0)	idx.value += 11;

			break;

		case TS_QUART:

			idx.value *= 3;

			if	(pos == 0)	idx.value += 1;
			else if	(pos > 0)	idx.value += 2;

			break;

		default:
			break;
		}

		switch (type)
		{
		case TS_YEAR:	idx.value /= 12; break;
		case TS_QUART:	idx.value /= 3; break;
		default:	break;
		}

		idx.type = type;
	}
	else
	{
		unsigned dat;

		if	(pos < 0)
			dat = TimeIndexFloor(idx);
		else if	(pos > 0)
			dat = TimeIndexCeil(idx);
		else	dat = mittel(TimeIndexFloor(idx), TimeIndexCeil(idx));

		switch (type)
		{
		case TS_YEAR:	idx.value = Calendar(dat, NULL)->year; break;
		case TS_QUART:	idx.value = dat2mon(dat) / 3; break;
		case TS_MONTH:	idx.value = dat2mon(dat); break;
		case TS_WEEK:	idx.value = (dat + 6) / 7; break;
		default:	idx.value = dat; break;
		}

		idx.type = type;
	}
	
	return idx;
}
