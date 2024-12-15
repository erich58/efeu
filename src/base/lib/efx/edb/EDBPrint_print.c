/*	Datenausgabe im Textformat
*/

#include <EFEU/EDB.h>
#include <EFEU/printobj.h>

static size_t write_print (EfiType *type, void *data, void *par)
{
	size_t n = PrintData(par, type, data);
	return n + io_nputc('\n', par, 1);
}

static void init_print (EDB *edb, EDBPrintMode *mode, IO *io)
{
	if	(mode->header)
	{
		edb_head(edb, io, mode->header);
		io_puts("@data print\n", io);
	}

	edb->write = write_print;
	edb->opar = io;
}

void EDBPrint_print (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->name = def->name;
	mode->init = init_print;
}
