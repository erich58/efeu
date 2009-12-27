/*	Argument aus Liste entfernen
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/


#include <EFEU/parsearg.h>
#include <EFEU/memalloc.h>


void skiparg(int *narg, char **arg, int n)
{
	for ((*narg)--; n <= *narg; n++)
		arg[n] = arg[n + 1];
}
