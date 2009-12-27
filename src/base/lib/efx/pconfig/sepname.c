/*	Namen abtrennen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/pconfig.h>

char *sepname(const char *str, char **ptr)
{
	int i;

	for (i = 0; str[i] != 0; i++)
	{
		if	(str[i] == '=')
		{
			if	(ptr)
			{
				if	(str[i+1] == 0)
				{
					*ptr = NULL;
				}
				else	*ptr = (char *) (str + i + 1);
			}

			if	(i != 0)
			{
				return mstrncpy(str, i);
			}
			else	return NULL;
		}
	}

	if	(ptr)
	{
		*ptr = (char *) str;
	}

	return NULL;
}
