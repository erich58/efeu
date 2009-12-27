/*	Kopf- und Fußzeilen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/object.h>
#include <EFEU/pconfig.h>
#include "printpar.h"

void headline(io_t *io, char *left, char *center, char *right)
{
	int ls, cs, rs, offset;

	left = parsub(left);
	center = parsub(center);
	right = parsub(right);

	if	(center)
	{
		cs = strlen(center);
		offset = 0;

		if	(cs > PageWidth)
		{
			offset = (cs - PageWidth) / 2;
			cs = PageWidth;
			memfree(left);
			memfree(right);
			left = right = NULL;
		}

		ls = (PageWidth - cs) / 2;
		rs = PageWidth - cs - ls; 
	}
	else
	{
		cs = 0;
		ls = left ? strlen(left) : 0;
		rs = right ? strlen(right) : 0;

		if	(ls + rs > PageWidth)
		{
			ls = PageWidth * (double) ls / (double) (ls + rs) + 0.5;
		}
		else	ls += (PageWidth - ls - rs) / 2;

		rs = PageWidth - ls;
		offset = 0;
	}

	io_printf(io, "%*.*s", -ls, 1 - ls, left);
	io_printf(io, "%*.*s", cs, cs, center + offset);
	io_printf(io, "%*.*s", rs, rs - 1, right);
	io_putc('\n', io);
	memfree(left);
	memfree(center);
	memfree(right);
}
