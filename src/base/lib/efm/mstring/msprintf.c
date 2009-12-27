/*	Printf für IO-Strukturen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/mstring.h>
#include <EFEU/io.h>


char *msprintf(const char *fmt, ...)
{
	va_list args;
	char *p;

	va_start(args, fmt);
	p = mvsprintf(fmt, args);
	va_end(args);
	return p;
}


char *mvsprintf(const char *fmt, va_list list)
{
	strbuf_t *sb;
	io_t *io;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	io_vprintf(io, fmt, list);
	io_close(io);
	return sb2str(sb);
}
