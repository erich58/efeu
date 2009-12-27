/*	Argumentwert abfragen
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/


#include <EFEU/parsearg.h>
#include <EFEU/mstring.h>


char *argval(const char *arg)
{
	if	(arg == NULL || (arg[0] == '-' && arg[1] == 0))
	{
		return NULL;
	}
	else	return mstrcpy(arg);
}
