/*	Standardhauptprogramm
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/procenv.h>

int procmain (int narg, char **arg);

int main (int narg, char **arg)
{
	procinit(arg[0]);
	procexit(procmain(narg, arg));
	return 0;
}
