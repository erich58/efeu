#ifndef	_out_tst_h
#define	_out_tst_h	1
#include <EFEU/UnitTest.h>

static STRBUF(data_buf, 256);
static IO *data_io = NULL;

static IO *out()
{
	sb_clean(&data_buf);

	if	(!data_io)
		data_io = io_strbuf(&data_buf);

	return data_io;
}

static const char *cstr(char *s)
{
	sb_clean(&data_buf);
	sb_puts(s, &data_buf);
	memfree(s);
	return sb_nul(&data_buf);
}

static const char *data()
{
	return sb_nul(&data_buf);
}

#endif
