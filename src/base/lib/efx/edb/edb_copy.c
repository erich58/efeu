/*
Kopieren von Datensätzen
*/

#include <EFEU/EDB.h>

int edb_copy (EDB *edb, int lim)
{
	int n = 0;

	if	(!edb || !edb->read || !edb->write)
		return 0;

	while (edb_read(edb))
	{
		edb_write(edb);
		n++;

		if	(lim && n >= lim)	break;
	}

	return n;
}
