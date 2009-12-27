/*	Zuweisungsargument bestimmen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/parsearg.h>
#include <EFEU/memalloc.h>
#include <ctype.h>


assignarg_t *assignarg (const char *arg)
{
	assignarg_t *x;
	char *p;
	size_t n;

	if	(arg == NULL)	return NULL;

	while (isspace(*arg))
		arg++;

	if	(arg[0] == 0 || (arg[0] == '-' && arg[1] == 0))
		return NULL;

	n = sizeof(assignarg_t) + strlen(arg) + 1;
	x = (assignarg_t *) memalloc(n);
	p = strcpy((char *) (x + 1), arg);
	x->name = p;
	x->opt = NULL;
	x->arg = NULL;

	while (*p != 0)
	{
		if	(*p == '[')
		{
			int depth = 0;

			*p = 0;
			p++;
			x->opt = p;

			for (; *p != 0; p++)
			{
				if	(*p == ']')
				{
					if	(!depth)
					{
						*p = 0;
						p++;
						break;
					}
					else	depth--;
				}
				else if	(*p == '[')	depth++;
			}
		}
		else if	(*p == '=')
		{
			*p = 0;
			p++;
			x->arg = p;
			break;
		}
		else	p++;
	}

	return x;
}
