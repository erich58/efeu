/*	Seitenformat bestimmen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/texutil.h>

static TeXpgfmt_t pgfmt[] = {
	{ "a4",		16.,	19.,	170.,	245. },
	{ "a4r",	15.,	18.,	250.,	160. },
	{ "schmal",	20.,	10.,	50.,	220. },
	{ "kurz",	20.,	15.,	220.,	50. },
	{ "klein",	20.,	10.,	50.,	50. },
};


void *TeXpglist(Func_t *func, void *data, void **arg)
{
	int i;
	char *delim;

	delim = NULL;

	for (i = 0; i < tabsize(pgfmt); i++)
	{
		io_puts(delim, iostd);
		delim = ", ";
		io_puts(pgfmt[i].name, iostd);
	}

	io_putc('\n', iostd);
	return NULL;
}


TeXpgfmt_t *TeXpgfmt(const char *name)
{
	int i;

	for (i = 0; i < tabsize(pgfmt); i++)
	{
		if	(strcmp(name, pgfmt[i].name) == 0)
		{
			return (pgfmt + i);
		}
	}

	return pgfmt;
}
