/*	Daten einer multidimensionale Matrix laden
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>


size_t md_getdata(io_t *io, const Type_t *type, mdaxis_t *axis, void *data)
{
	size_t i, n;

	if	(x != NULL)
	{
		for (i = n = 0; i < axis->dim; i++)
		{
			n += md_getdata(io, type, axis->next, data);
			data = ((char *) data) + x->size;
		}
	}
	else	return io_read(io, data, type->size);

	return n;
}
