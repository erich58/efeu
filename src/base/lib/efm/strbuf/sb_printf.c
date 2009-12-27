/*	Printf für Stringbuffer
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/strbuf.h>
#include <EFEU/io.h>


int sb_printf (strbuf_t *sb, const char *fmt, ...)
{
	if	(sb && fmt)
	{
		va_list args;
		io_t *io;
		int n;

		io = io_strbuf(sb);
		va_start(args, fmt);
		n = io_vprintf(io, fmt, args);
		va_end(args);
		io_close(io);
		return n;
	}
	else	return 0;
}

int sb_vprintf (strbuf_t *sb, const char *fmt, va_list list)
{
	if	(sb && fmt)
	{
		io_t *io;
		int n;

		io = io_strbuf(sb);
		n = io_vprintf(io, fmt, list);
		io_close(io);
		return n;
	}
	else	return 0;
}
