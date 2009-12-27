/*	Datenmatrix konvertieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>

int md_konv(mdmat_t *md, const Type_t *type)
{
	Konv_t def;
	char *buf, *src, *tg;
	ulong_t dim;
	mdaxis_t *x;

	if	(md == NULL)		return EOF;
	if	(type == NULL)		return 0;
	if	(md->type == type)	return 0;

	if	(GetKonv(&def, md->type, type) == NULL)
	{
		errmsg(MSG_MDMAT, 102);
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

	md->type = (Type_t *) type;
	memfree(md->data);
	md->data = buf;
	return 1;
}
