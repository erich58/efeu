/*	IO-Strukturen umkopieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/parsub.h>

int iocopystr (const char *fmt, io_t *out, iocopy_t copy, void *arg)
{
	io_t *in;
	int n;

	in = io_cstr(fmt);
	n = copy(in, out, arg);
	io_close(in);
	return n;
}


char *miocopy (io_t *in, iocopy_t copy, void *arg)
{
	strbuf_t *sb;
	io_t *out;

	sb = new_strbuf(0);
	out = io_strbuf(sb);
	copy(in, out, arg);
	io_close(out);
	return sb2str(sb);
}


char *miocopystr (const char *fmt, iocopy_t copy, void *arg)
{
	strbuf_t *sb;
	io_t *in, *out;

	sb = new_strbuf(0);
	in = io_cstr(fmt);
	out = io_strbuf(sb);
	copy(in, out, arg);
	io_close(out);
	io_close(in);
	return sb2str(sb);
}
