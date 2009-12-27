/*	Datenbankdaten anfordern/freigeben
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/DBType.h>

void *NewDBData (DBType_t *dbd, size_t n)
{
	if	(dbd == NULL || n == 0)	return NULL;

	return memalloc(dbd->size * n);
}

void DelDBData (DBType_t *dbd, void *data, size_t n)
{
	ClearDBData(dbd, data, n);
	memfree(data);
}
