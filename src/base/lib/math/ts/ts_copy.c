/*	Zeitreihen kopieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <Math/TimeSeries.h>


TimeSeries_t *CopyTimeSeries (const char *name, const TimeSeries_t *ts,
	int a, int b)
{
	TimeSeries_t *x;
	int i;

	if	(ts == NULL)	return NULL;
	if	(b < a)		b = a + 1;

	x = NewTimeSeries(name ? name : ts->name, ts->base, b - a);
	x->fmt = mstrcpy(ts->fmt);
	x->base.value += a;

	if	(b < 0)	return x;

	b = min(b, ts->dim);

	for (i = max(0, a); i < b; i++)
		x->data[i - a] = ts->data[i];

	return x;
}
