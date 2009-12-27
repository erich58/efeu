/*	Binäre Datenausgabe
*/

#include <EFEU/EDB.h>

static size_t write_null (EfiType *type, void *data, void *par)
{
	return 1;
}

static void init_null (EDB *edb, EDBPrintMode *mode, IO *io)
{
	edb->write = write_null;
	edb->opar = io;
}

void EDBPrint_null (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->name = def->name;
	mode->init = init_null;
}
