#include <EFEU/UnitTest.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/Pixmap.h>
#include <EFEU/strbuf.h>
#include <EFEU/procenv.h>

/* Testeingabe
*/

static StrBuf buf;

static int in_get (IO *io)
{
	return sb_get(&buf);
}

static int in_ctrl (IO *io, int req, va_list list)
{
	switch (req)
	{
	case IO_ERROR:	return 0;	// Kein exit(1) bei Fehler!
	default:	return EOF;
	}
}

static IO input = STD_IODATA (in_get, NULL, in_ctrl, "<in>");

static const char *set_input(const char *data_)
{
	IO *io = io_cstr(data_);
	int c;

	sb_trunc(&buf);

	while ((c = io_xgetc(io, NULL)) != EOF)
		sb_putc(c, &buf);

	sb_begin(&buf);
	io_close(io);
	return data_;
}

#define	INPUT(str)	\
UnitTest_strinfo(__FILE__, __LINE__, "INFO", \
set_input(str), "input")

static const char *load_and_dump()
{
	size_t c, r;
	unsigned char *p;

	EPixmap *pix = LoadPixmap(&input);
	sb_trunc(&buf);

	if	(!pix)
		return NULL;

	sb_printf(&buf, "%dx%d\n", pix->width, pix->height);
	p = pix->data;

	for (r = 0; r < pix->height; r++)
	{
		for (c = 0; c < pix->width; c++)
		{
			if	(c)
				sb_putc(' ', &buf);

			sb_printf(&buf, "%02X%02X%02X", p[0], p[1], p[2]);
			p += 3;
		}

		sb_putc('\n', &buf);
	}

	return sb_nul(&buf);
}

#define	OUTPUT(out)	\
UnitTest_strcheck(__FILE__, __LINE__, "==", \
	load_and_dump(), out, "output", # out)


TEST_DECL(bad_magic)
{
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("efmain.msg", "ppm	invalid header\n");

	INPUT("A\n");
	UnitTest_stderr("error");
	OUTPUT(NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("error", "ERR invalid header\n");

	INPUT("PX\n");
	UnitTest_stderr("error");
	OUTPUT(NULL);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("error", "ERR invalid header\n");
}

TEST_DECL(pbm)
{
	INPUT(
		"P1\n"
		"#comment\n"
		"8 1\n"
		"1100 1100"
	);
	OUTPUT(
		"8x1\n"
		"000000 000000 FFFFFF FFFFFF 000000 000000 FFFFFF FFFFFF\n"
	);

	INPUT(
		"P4\n"
		"#comment\n"
		"8 1\n"
		"\314"
	);
	OUTPUT(
		"8x1\n"
		"000000 000000 FFFFFF FFFFFF 000000 000000 FFFFFF FFFFFF\n"
	);
}

TEST_DECL(pgm)
{
	INPUT(
		"P2\n"
		"#comment\n"
		"4 1\n"
		"255\n"
		"128 128 128 128\n"
	);
	OUTPUT(
		"4x1\n"
		"808080 808080 808080 808080\n"
	);
	INPUT(
		"P5\n"
		"#comment\n"
		"4 1\n"
		"255\n"
		"\x80\x80\x80\x80"
	);
	OUTPUT(
		"4x1\n"
		"808080 808080 808080 808080\n"
	);
}

TEST_DECL(ppm)
{
	INPUT(
		"P3\n"
		"#comment\n"
		"1 1\n"
		"#comment\n"
		"127\n"
		"112 113 114\n"
	);
	OUTPUT(
		"1x1\n"
		"E1E3E5\n"
	);
	INPUT(
		"P6\n"
		"#comment\n"
		"1 1\n"
		"#comment\n"
		"127\n"
		"\x70\x71\x72"
	);
	OUTPUT(
		"1x1\n"
		"E1E3E5\n"
	);
}

TEST_SUITE
{
	TEST_RUN(bad_magic);
	TEST_RUN(pbm);
	TEST_RUN(pgm);
	TEST_RUN(ppm);
}
