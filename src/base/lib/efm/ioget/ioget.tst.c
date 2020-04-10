#include <EFEU/UnitTest.h>
#include <EFEU/io.h>

#define	EXPECT_BUF(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", sb_nul(a), b, # a, # b)

#define	EXPECT_DATA(a, b) \
	UnitTest_strcheck(__FILE__, __LINE__, "==", DATA(a), b, # a, # b)

static IO *IN(char *def)
{
	static IO *io = NULL;
	io_close(io);
	io = def ? io_string(def, NULL) : NULL;
	return io;
}

static char *DATA(char *def)
{
	static char *buf = NULL;
	memfree(buf);
	buf = def ? def : NULL;
	return buf;
}

#define	EXPECT_GETARG(def, beg, end, res) \
	EXPECT_DATA(io_getarg(IN(def), beg, end), res)

TEST_DECL(getarg)
{
	EXPECT_GETARG("[a(b)c]", '[', ']', "a(b)c");
	EXPECT_GETARG("[a(bc]", '[', ']', "a(bc]");
	EXPECT_GETARG("[a(b{c)d]", '[', ']', "a(b{c)d]");
	EXPECT_GETARG("|a[bc|d]|ef", '|', '|', "a[bc|d]");
	EXPECT_GETARG("x|a", '|', '|', NULL);
}

TEST_DECL(getc)
{
	StrBuf *sb;
	IO *io;
	int i;
	
	EXPECT(io_peek(NULL) == EOF);
	EXPECT(io_getc(NULL) == EOF);
	EXPECT(io_ungetc('a', NULL) == 0);

	EXPECT(io_peek(ionull) == EOF);
	EXPECT(io_getc(ionull) == EOF);
	EXPECT(io_ungetc('a', ionull));
	EXPECT(io_peek(ionull) == 'a');
	EXPECT(io_getc(ionull) == 'a');

	EXPECT_PTR(io = IN("abc"));
	EXPECT(io_peek(io) == 'a');
	EXPECT(io_getc(io) == 'a');
	EXPECT(io_ungetc('x', io));
	EXPECT(io_peek(io) == 'x');
	EXPECT(io_getc(io) == 'x');

	sb = sb_create(32);
	EXPECT(sb_puts("1", sb) == 1);
	sb_setpos(sb, 0);
	io = io_strbuf(sb);
	EXPECT(io_peek(io) == '1');
	EXPECT(io_getc(io) == '1');
	EXPECT(io_peek(io) == EOF);
	EXPECT(io_getc(io) == EOF);

	for (i = 0; i < IO_MAX_SAVE; i++)
		EXPECT(io_ungetc('x', io));

	EXPECT(io_ungetc('a', io));
	EXPECT(sb->data[0] == 'x');
	EXPECT(io_ungetc('a', io) == 0);

	sb_clean(sb);
	EXPECT(sb_puts("1", sb) == 1);
	io_rewind(io);
	io_push(io, io_string("x", NULL));
	EXPECT(io_getc(io) == 'x');
	EXPECT(io_getc(io) == '1');
	EXPECT(io_getc(io) == EOF);

	io_rewind(io);
	io_push(io, io_string("x", NULL));
	EXPECT(io_getc(io) == 'x');
	EXPECT(io_peek(io) == '1');

	sb_destroy(sb);
}

TEST_SUITE
{
	TEST_RUN(getarg);
	TEST_RUN(getc);
	// Speicherfreigabe
	IN(NULL);
	DATA(NULL);
}
