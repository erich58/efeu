/*	Fehlermeldung mit IO-Struktur
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/procenv.h>

void io_message(io_t *io, const char *name, int num, int narg, ...)
{
	va_list list = va_start(list, narg);
	char *id = io_ident(io);
	vmessage(id, name, num, narg, list);
	memfree(id);
	va_end(list);
}

void io_error(io_t *io, const char *name, int num, int narg, ...)
{
	va_list list = va_start(list, narg);
	char *id = io_ident(io);
	vmessage(id, name, num, narg, list);
	memfree(id);
	va_end(list);

	if	(io_ctrl(io, IO_ERROR) == EOF)
		procexit(EXIT_FAILURE);
}

