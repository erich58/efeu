/*
Ausgabestruktur
*/

#include <EFEU/EDB.h>

void edb_setout (EDB *edb, IO *out, EDBPrintMode *pmode)
{
	if	(edb && pmode && out)
	{
		if	(pmode->filter)
			out = pmode->filter(out);

		pmode->init(edb, pmode, out);
	}
}

void edb_out (EDB *edb, IO *io, const char *mode)
{
	edb_setout(edb, io, edb_pmode(NULL, edb, mode));
}
