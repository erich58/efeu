/*	Skalar in Datenmatrix konvertieren
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>

mdmat *md_skalar (EfiObj *obj)
{
	if	(obj)
	{
		mdmat *md = new_mdmat();
		md->type = obj->type;
		md->sbuf = NewStrPool();
		md_alloc(md);
		CopyData(obj->type, md->data, obj->data);
		rd_deref(obj);
		return md;
	}

	return NULL;
}
