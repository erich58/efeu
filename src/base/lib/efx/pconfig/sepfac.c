/*	Multiplikator abtrennen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/pconfig.h>


double sepfactor(const char *str, char **ptr)
{
	double val;
	char *p;

	val = strtod(str, &p);

	if	(p == str)
	{
		if	(*p == '-')
		{
			val = -1.;
			p++;
		}
		else	val = 1.;
	}
	else if	(*p == '*')
	{
		p++;
	}

	if	(ptr)	*ptr = p;

	return val;
}
