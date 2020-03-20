#include <EFEU/UnitTest.h>
#include <EFEU/StrPool.h>
#include <EFEU/memalloc.h>
#include <EFEU/io.h>

#include "utest/data.tst.h"

TEST_DECL(base)
{
	StrPool *s = NewStrPool();
	EXPECT_STR(cstr(rd_ident(s)), "8/8 + 0/0");
	EXPECT(StrPool_add(s, "abc") == 8);
	EXPECT(StrPool_add(s, "0123456789") == 8 + 4);
	EXPECT(StrPool_offset(s) == 23);
	EXPECT(StrPool_align(s, 4) == 16);
	EXPECT(StrPool_align(s, 8) == 16);
	EXPECT(StrPool_offset(s) == 24);
	EXPECT(StrPool_add(s, NULL) == 0);
	EXPECT(StrPool_xadd(s, NULL) == 24);
	EXPECT(StrPool_nadd(s, "abcdefg", 3) == 24 + 1);
	EXPECT_STR(cstr(rd_ident(s)), "8/8 + 21/1000");
	EXPECT(StrPool_offset(s) == 29);

	EXPECT_STR(StrPool_get(s, 0), NULL);
	EXPECT_STR(StrPool_mget(s, 0), NULL);
	EXPECT_STR(StrPool_get(s, 8), "abc");
	EXPECT_STR(StrPool_get(NULL, 8), NULL);
	EXPECT_STR(StrPool_get(s, 12), "0123456789");
	EXPECT_STR(StrPool_get(s, 23), "");
	EXPECT_STR(StrPool_get(s, 24), "");
	EXPECT_STR(StrPool_get(s, 25), "abc");
	EXPECT_STR(StrPool_get(s, 28), "");
	EXPECT_STR(StrPool_get(s, 29), NULL);
	StrPool_save(s, UnitTest_tmpname("data"));

	StrPool *t = StrPool_map(UnitTest_tmpname("data"));
	EXPECT_STR(cstr(rd_ident(t)), "8/29 + 0/0");
	EXPECT(StrPool_start(t) == 0);
	EXPECT(StrPool_next(t) == 8);
	EXPECT(StrPool_next(t) == 12);
	EXPECT(StrPool_next(t) == 23);
	EXPECT(StrPool_next(t) == 24);
	EXPECT(StrPool_next(t) == 25);
	EXPECT(StrPool_next(t) == 0);
	EXPECT(StrPool_copy(s, s, 8) == 8);
	EXPECT(StrPool_copy(t, s, 8) == 29);
	EXPECT_STR(StrPool_get(t, 29), "abc");
	EXPECT_STR(cstr(rd_ident(t)), "29/29 + 4/1000");
	EXPECT(StrPool_align(t, 2000) == 2000);
	EXPECT(StrPool_offset(t) == 2029);
	EXPECT_STR(cstr(rd_ident(t)), "29/29 + 2000/2000");

	EXPECT(StrPool_cmp(s, 8, s, 8) == 0);
	EXPECT(StrPool_cmp(s, 8, t, 29) == 0);
	EXPECT(StrPool_cmp(s, 12, t, 29) < 0);
	EXPECT(StrPool_cmp(s, 0, NULL, 29) == 0);

	rd_deref(s);
	rd_deref(t);
}

TEST_DECL(rw)
{
	StrBuf *sb = sb_create(64);
	sb_puts("abc", sb);
	sb_putc(0, sb);
	sb_puts("1234567890", sb);
	sb_putc(0, sb);
	sb_putc(0, sb);
	sb_puts("xyz", sb);
	sb_putc(0, sb);
	EXPECT(sb_size(sb) == 20);

	StrPool *s = StrPool_alloc(20);
	sb_setpos(sb, 0);
	IO *io = io_strbuf(sb);
	EXPECT(StrPool_read(s, 10, io) == 10);
	EXPECT(StrPool_read(s, 10, io) == 10);
	EXPECT(StrPool_read(s, 10, io) == 0);
	io_close(io);
	sb_destroy(sb);

	EXPECT(StrPool_add(s, "ABC")== 28);

	StrPool_dump(s, out());
	EXPECT_STR(data(),
		"  ----- ------------\n"
		"      0 \"StrPool\"\n"
		"      8 \"abc\"\n"
		"     12 \"1234567890\"\n"
		"     23 \"\"\n"
		"     24 \"xyz\"\n"
		"  ----- ------------\n"
		"     28 \"ABC\"\n"
		"  ----- ------------\n");

	EXPECT(StrPool_write(s, out()) == 32);
	EXPECT_STR(data(), "StrPool");
	EXPECT_STR(data() + 8, "abc");
	EXPECT_STR(data() + 12, "1234567890");
	EXPECT_STR(data() + 23, "");
	EXPECT_STR(data() + 24, "xyz");
	EXPECT_STR(data() + 28, "ABC");
	EXPECT_STR(data() + 32, "");

	rd_deref(s);
}

static int ext_clean(void *data, size_t size)
{
	io_printf(out(), "clean: %zd", size);
	return 0;
}

TEST_DECL(ext)
{
	char buf[] = "StrPool\0abc\0\0xy";
	StrPool *pool = StrPool_ext(buf, sizeof buf, ext_clean);
	StrPool_dump(pool, out());
	EXPECT_STR(data(),
		"  ----- ------------\n"
		"      0 \"StrPool\"\n"
		"      8 \"abc\"\n"
		"     12 \"\"\n"
		"     13 \"xy\"\n"
		"  ----- ------------\n"
		"  ----- ------------\n");
	rd_deref(pool);
	EXPECT_STR(data(), "clean: 16");
}

TEST_DECL(out)
{
	StrPool *pool = NewStrPool();
	IO *io = StrPool_open(pool);

	io_puts("ABC", io);
	EXPECT_STR(rd_ident(io), "<StrPool>");
	EXPECT(io_rewind(io) == EOF);
	io_close(io);

	StrPool_printf(pool, "-%d-", 5);
	StrPool_psubarg(pool, "$0:$1", "sd", "xyz", 5);

	StrPool_dump(pool, out());
	EXPECT_STR(data(),
		"  ----- ------------\n"
		"      0 \"StrPool\"\n"
		"  ----- ------------\n"
		"      8 \"ABC\"\n"
		"     12 \"-5-\"\n"
		"     16 \"xyz:5\"\n"
		"  ----- ------------\n");
	rd_deref(pool);

	EXPECT(StrPool_open(NULL) == NULL);
}

TEST_SUITE
{
	TEST_RUN(base);
	TEST_RUN(rw);
	TEST_RUN(ext);
	TEST_RUN(out);
}
