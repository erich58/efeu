/*	Skalar in Datenmatrix konvertieren
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

mdmat *md_skalar (const EfiObj *obj)
{
	if	(obj)
	{
		mdmat *md = new_mdmat();
		md->type = (EfiType *) obj->type;
		md->sbuf = NewStrPool();
		md_alloc(md);
		CopyData(obj->type, md->data, obj->data);
		return md;
	}

	return NULL;
}
