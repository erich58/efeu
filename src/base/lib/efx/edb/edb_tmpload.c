/*	Tempor�res zwischenladen eines Datenfiles
	$Copyright (C) 2006 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>

static int read_binary (EfiType *type, void *data, void *par)
{
	if	(io_peek(par) == EOF)
		return 0;

	return ReadData(type, data, par) != 0;
}

void edb_tmpload (EDB *edb)
{
	IO *tmp;

	if	(!(edb && edb->read))
		return;

	tmp = io_tmpfile();

	if	(edb->save)
		WriteData(edb->obj->type, edb->obj->data, tmp);

	edb->save = 0;

	while (edb->read(edb->obj->type, edb->obj->data, edb->ipar))
		WriteData(edb->obj->type, edb->obj->data, tmp);

	CleanData(edb->obj->type, edb->obj->data);
	edb_closein(edb);
	io_rewind(tmp);
	edb->read = read_binary;
	edb->ipar = tmp;
}
