/*	Datenbanktypefunktionen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/DBType.h>

size_t ReadDBData (DBType_t *x, io_t *io, void *data, size_t n)
{
	if	(!x || !data)	return 0;
	if	(x->read)	return x->read(io, data, n);

	return io_dbread(io, data, x->recl, x->size, n);
}

size_t WriteDBData (DBType_t *x, io_t *io, void *data, size_t n)
{
	if	(!x || !data)	return 0;
	if	(x->write)	return x->write(io, data, n);

	return io_dbwrite(io, data, x->recl, x->size, n);
}

void AllocDBData (DBType_t *dbd, void *data, size_t n)
{
	if (dbd && data && n && dbd->alloc) dbd->alloc(data, n);
}

void SyncDBData (DBType_t *dbd, void *data, size_t n)
{
	if (dbd && data && n && dbd->sync) dbd->sync(data, n);
}

void ClearDBData (DBType_t *dbd, void *data, size_t n)
{
	if	(dbd && data && n)
	{
		if	(dbd->clear)	dbd->clear(data, n);
		else			memset(data, 0, n * dbd->size);
	}
}
