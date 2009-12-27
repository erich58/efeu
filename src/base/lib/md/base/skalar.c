/*	Skalar in Datenmatrix konvertieren
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

mdmat_t *md_skalar (const Type_t *type, const void *data)
{
	mdmat_t *md;

	md = new_mdmat();
	md->type = (Type_t *) type;
	md->size = md->type->size;
	md->data = memalloc(md->type->size);
	CopyData(type, md->data, data);
	return md;
}
