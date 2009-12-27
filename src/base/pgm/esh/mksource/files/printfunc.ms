/*	Ausgabefunktionen
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#ifndef	_MS_PRINTFUNC
#define	_MS_PRINTFUNC	1

#include <function.ms>


void PrintFunc(str name, str args, IO def, bool glob = true)
{
	Function f = Function(name, paste(", ", "io_t *out", args));
	f.pfbody("out", def);
	f.write(glob);
}

#endif	/* MS_PRINTFUNC */
