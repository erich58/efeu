/*	Umkopieren auf String
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/parsub.h>


/*	Parametersubstitution
*/

char *miocpy(io_t *in, iocpy_t *def, size_t dim)
{
	strbuf_t *sb;
	io_t *tmp;

	sb = new_strbuf(0);
	tmp = io_strbuf(sb);
	iocpy(in, tmp, def, dim);
	io_close(tmp);

	if	(iocpy_flag)
		sb_putc(0, sb);

	return sb2str(sb);
}


char *miocpyfmt(const char *fmt, iocpy_t *def, size_t dim)
{
	io_t *in;
	char *p;

	in = io_cstr(fmt);
	p = miocpy(in, def, dim);
	io_close(in);
	return p;
}
