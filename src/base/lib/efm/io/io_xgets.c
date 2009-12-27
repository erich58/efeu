/*	String lesen/kopieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/iocpy.h>


char *io_xgets(io_t *io, const char *delim)
{
	int c;
	strbuf_t *sb;

	sb = new_strbuf(0);
	io_protect(io, 1);

	while ((c = io_xgetc(io, delim)) != EOF)
		sb_putc(c, sb);

	io_protect(io, 0);
	sb_putc(0, sb);
	return sb2mem(sb);
}


int io_xcopy(io_t *in, io_t *out, const char *delim)
{
	int n, c;

	io_protect(in, 1);
	n = 0;

	while ((c = io_xgetc(in, delim)) != EOF)
		if (io_putc(c, out) != EOF) n++;

	io_protect(in, 0);
	return n;
}


int iocpy_xstr(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	register int n = io_xcopy(in, out, arg);
	io_getc(in);
	return n;
}
