/*	Binäre Datenausgabe
*/

#include <EFEU/EDB.h>

static size_t write_binary (EfiType *type, void *data, void *par)
{
	return WriteData(type, data, par);
}

static void init_binary (EDB *edb, EDBPrintMode *mode, IO *io)
{
	if	(!mode->header)
		mode->header = 1;

	edb_head(edb, io, mode->header);
	io_puts("@data binary\n", io);
	edb->write = write_binary;
	edb->opar = io;
}

void EDBPrint_binary (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->name = def->name;
	mode->init = init_binary;
}
