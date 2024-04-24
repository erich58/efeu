/*	Daten serialisieren
	(c) 2024 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <assert.h>

static int check_size(mdaxis *axis, size_t size)
{
	while (axis)
	{
		if	(axis->dim * axis->size != size)
			return 0;

		size = axis->size;
		axis = axis->next;
	}

	return 1;
}

static size_t save_data(FILE *file, mdaxis *axis, const char *data, size_t size)
{
	if	(axis)
	{
		size_t s = 0;

		for (size_t n = 0; n < axis->dim; ++n)
		{
			s += save_data(file, axis->next, data, size);
			data += axis->size;
		}

		return s;
	}
	else
	{
		return fwrite(data, 1, size, file);
	}
}

int md_serialize(mdmat *md)
{
	if	(!md)	return 0;

	if	(check_size(md->axis, md->size))
		return 0;

	FILE *file = tmpfile();
	assert(save_data(file, md->axis, md->data, md->type->size) == md->size);
	rewind(file);
	assert(fread(md->data, 1, md->size, file) == md->size);
	assert(md->size == md_size(md, md->axis));
	return 1;
}
