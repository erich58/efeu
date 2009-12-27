/*	IO-Basiskontrolle
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>


/*	Kontrollfunktion
*/

int io_vctrl (io_t *io, int req, va_list list)
{
	return (io && io->ctrl) ? io->ctrl(io->data, req, list) : EOF;
}

int io_ctrl (io_t *io, int req, ...)
{
	if	(io && io->ctrl)
	{
		va_list list;
		int stat;

		va_start(list, req);
		stat = (*io->ctrl)(io->data, req, list);
		va_end(list);
		return stat;
	}
	else	return EOF;
}
