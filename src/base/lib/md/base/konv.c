/*	Datenmatrix konvertieren
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

int md_konv(mdmat *md, const EfiType *type)
{
	EfiKonv def;
	char *buf, *src, *tg;
	size_t dim;
	mdaxis *x;

	if	(md == NULL)		return EOF;
	if	(type == NULL)		return 0;
	if	(md->type == type)	return 0;

	if	(GetKonv(&def, md->type, type) == NULL)
	{
		log_note(NULL, "[mdmat:102]", NULL);
		return EOF;
	}

	dim = md->size / md->type->size;
	md->size = dim * type->size;
	buf = memalloc(md->size);
	memset(buf, 0, md->size);
	src = md->data;
	tg = buf;

	while (dim-- > 0)
	{
		KonvData(&def, tg, src);
		tg += type->size;
		src += md->type->size;
	}

	for (x = md->axis; x != NULL; x = x->next)
	{
		x->size /= md->type->size;
		x->size *= type->size;
	}

	md->type = (EfiType *) type;
	memfree(md->data);
	md->data = buf;
	return 1;
}
