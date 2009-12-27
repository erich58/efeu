/*	Datenvektor laden
*/

#include <EFEU/EDB.h>

void EfiVec_load (EfiVec *vec, const char *name,
	const char *mode, const char *filter)
{
	EDB *edb;
	char *p;
	size_t n;
	
	if	(!vec)	return;

	if	(mode)
	{
		edb = edb_alloc(LvalObj(NULL, vec->type), NULL);
		edb_data(edb, io_fileopen(name, "rz"), mode);
		edb = edb_filter(edb, filter);
	}
	else
	{
		edb = edb_filter(edb_fopen(NULL, name), filter);
		edb = edb_conv(edb, vec->type);
	}

	for (n = 0; edb_read(edb); n++)
	{
		if	(n < vec->buf.used)
		{
			p = (char *) vec->buf.data + vec->buf.elsize * n;
			AssignData(vec->type, p, edb->obj->data);
		}
		else if	(vec->buf.blksize)
		{
			CopyData(vec->type, vb_next(&vec->buf), edb->obj->data);
		}
		else	break;
	}

	rd_deref(edb);
}

void EfiVec_save (EfiVec *vec, const char *name,
	const char *mode, const char *desc)
{
	EfiObj *obj;
	EDB *edb;
	size_t n;
	char *p;
	
	if	(!vec)	return;

	obj = LvalObj(NULL, vec->type);
	edb = edb_alloc(obj, mstrcpy(desc));
	n = vec->buf.used;
	p = vec->buf.data;
	edb_fout(edb, name, mode);

	while (n-- > 0)
	{
		AssignData(obj->type, obj->data, p);
		p += vec->buf.elsize;
		edb_write(edb);
	}

	rd_deref(edb);
}
