/*	Skalar in Datenmatrix konvertieren
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

mdmat *md_skalar (const EfiType *type, const void *data)
{
	mdmat *md;

	md = new_mdmat();
	md->type = (EfiType *) type;
	md->size = md->type->size;
	md->data = memalloc(md->type->size);
	CopyData(type, md->data, data);
	return md;
}
