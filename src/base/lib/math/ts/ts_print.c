/*	Zeitreihenstruktur ausgeben
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/object.h>
#include <Math/TimeSeries.h>

int PrintTimeSeries(io_t *io, const TimeSeries_t *ts, const char *fmt)
{
	char *p;
	int i, n;

	if	(ts == NULL)	return 0;

	n = io_printf(io, "%s[%d] ", ts->name, ts->dim);

	p = TimeIndex2str(ts->base, 0);
	n += io_puts(p, io);
	memfree(p);

	if	(ts->fmt)	fmt = ts->fmt;
	if	(!fmt)		fmt = "%#10.2f";

	if	(ts->dim > 0)
	{
		n += io_puts(" ", io);
		p = TimeIndex2str(ts->base, ts->dim - 1);
		n += io_puts(p, io);
		memfree(p);

		for (i = 0; i < ts->dim; i++)
		{
			if	(i == 0)	n += io_puts(" {\n", io);
			else if	(i % 6 == 0)	n += io_puts("\n", io);
			else			n += io_puts(" ", io);

			n += io_printf(io, fmt, ts->data[i]);
		}

		n += io_puts("\n}\n", io);
	}
	else	n += io_puts(" { }\n", io);

	return n;
}
