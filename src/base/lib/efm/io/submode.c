/*	Geschützte Eingabe
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

void io_linemark (io_t *io)
{
	io_ctrl(io, IO_LINEMARK);
}

void io_submode (io_t *io, int flag)
{
	io_ctrl(io, IO_SUBMODE, flag);
}

void io_protect (io_t *io, int flag)
{
	io_ctrl(io, IO_PROTECT, flag);
}

extern char *io_prompt (io_t *io, const char *prompt)
{
	return io_ctrl(io, IO_PROMPT, &prompt) == EOF ? NULL : (char *) prompt;
}
