/*	Zeitreihenstruktur ausgeben
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <Math/TimeSeries.h>

double TimeSeriesValue (TimeSeries_t *ts, TimeIndex_t idx, double defval)
{
	if	(ts == NULL)
		return defval;

	if	(idx.type != ts->base.type)
		idx = TimeIndexKonv(idx, ts->base.type, 0);

	if	(idx.value <= ts->base.value)
		return ts->data[0];

	idx.value -= ts->base.value;

	if	(idx.value < ts->dim)
		return ts->data[idx.value];

	return ts->data[ts->dim - 1];
}
