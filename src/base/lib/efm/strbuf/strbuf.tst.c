#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>
#include <EFEU/LogConfig.h>
#include <fcntl.h>
#include <unistd.h>

TEST_DECL(expand)
{
	static STRBUF(buf, 32);
	EXPECT_TRUE(sb_expand(&buf, 100) != NULL);
	EXPECT_TRUE(buf.size == 128);	// 4 * 32
	EXPECT_TRUE(buf.blksize == 32);

	EXPECT_TRUE(sb_expand(&buf, 200) != NULL);
	EXPECT_TRUE(buf.size == 224);	// 7 * 32
	EXPECT_TRUE(buf.blksize == 128);

	EXPECT_TRUE(sb_expand(&buf, 500) != NULL);
	EXPECT_TRUE(buf.size == 608);	// 224 + 3 * 128
	EXPECT_TRUE(buf.blksize == 224);

	sb_free(&buf);
	EXPECT_TRUE(buf.size == 0);
	EXPECT_TRUE(buf.blksize == 224);

	sb_init(&buf, 0);
	EXPECT_TRUE(buf.size == 0);
	EXPECT_TRUE(buf.blksize == 0);

	EXPECT_TRUE(sb_expand(&buf, 1) != NULL);
	EXPECT_TRUE(buf.size == 1000);
	EXPECT_TRUE(buf.blksize == 1000);

	sb_free(&buf);
}

TEST_DECL(getmem)
{
	static STRBUF(buf, 32);
	sb_puts("TestDaten", &buf);
	EXPECT_STR(sb_nul(&buf), "TestDaten");
	char *p;

	EXPECT_STR((p = sb_strcpy(&buf)), "TestDaten");
	memfree(p);
	sb_putc(0, &buf);
	EXPECT_STR((p = sb_memcpy(&buf)), "TestDaten");
	memfree(p);
	EXPECT_STR((p = sb_getmem(&buf)), "TestDaten");
	memfree(p);

	EXPECT_STR(sb_getmem(&buf), NULL);
	EXPECT_STR(sb_getmem(NULL), NULL);
	EXPECT_STR(sb_strcpy(&buf), NULL);
	EXPECT_STR(sb_strcpy(NULL), NULL);
}

TEST_DECL(pos)
{
	static STRBUF(buf, 32);
	EXPECT_TRUE(sb_putstr("abcdef", NULL) == 0);
	EXPECT_TRUE(sb_putstr("abcdef", &buf) == 7);

	EXPECT_TRUE(sb_setpos(NULL, 3) == 0);
	EXPECT_TRUE(sb_setpos(&buf, -1) == 0);
	EXPECT_TRUE(sb_setpos(&buf, 20) == 7);
	EXPECT_TRUE(sb_setpos(&buf, 3) == 3);
	sb_puts("123456", &buf);
	EXPECT_STR(sb_nul(&buf), "abc123456");
	sb_trunc(&buf);
	EXPECT_STR(sb_nul(&buf), "");
	EXPECT_STR(sb_nul(NULL), NULL);
}

TEST_DECL(acquire)
{
	StrBuf *buf = sb_acquire();
	sb_release(buf);
	EXPECT_TRUE(sb_acquire() == buf);

	sb_putstr("abc", buf);
	char *p = sb_cpyrelease(buf);
	EXPECT_STR(p, "abc");
	memfree(p);

	StrBuf *tab[5];
	
	for (int i = 0; i < 5; i++)
		tab[i] = sb_acquire();
	
	for (int i = 0; i < 5; i++)
		sb_release(tab[i]);
}

TEST_DECL(edit)
{
	static STRBUF(buf, 4);
	EXPECT_TRUE(sb_insert(NULL, 'a', 3) == NULL); 
	EXPECT_TRUE(memcmp(sb_insert(&buf, 'a', 3), "aaa", 3) == 0); 
	sb_setpos(&buf, 2);
	EXPECT_TRUE(memcmp(sb_insert(&buf, 'x', 2), "xx", 2) == 0); 
	sb_end(&buf);
	EXPECT_STR(sb_nul(&buf), "aaxxa");
	EXPECT_TRUE(sb_delete(NULL, 1) == NULL); 
	EXPECT_TRUE(sb_delete(&buf, 1) == NULL); 
	sb_setpos(&buf, 1);
	EXPECT_TRUE(memcmp(sb_delete(&buf, 2), "ax", 2) == 0); 
	sb_end(&buf);
	EXPECT_STR(sb_nul(&buf), "axa");
	sb_puts("12345", &buf);
	EXPECT_TRUE(sb_move(&buf, 1, 5, 2) == 2);
	EXPECT_STR(sb_nul(&buf), "a34xa125");
	EXPECT_TRUE(sb_move(&buf, 4, 6, 0) == 2);
	EXPECT_STR(sb_nul(&buf), "a34x25a1");
	sb_nputc('z', &buf, 1000);
	EXPECT_TRUE(sb_size(&buf) == 1008);
	EXPECT_TRUE(sb_move(&buf, 5, 8, 0) == 1000);
	sb_setpos(&buf, 10);
	EXPECT_STR(sb_nul(&buf), "a34x2zzzzz");
	EXPECT_TRUE(sb_swap(&buf, 2, 4) == 4);
	EXPECT_STR(sb_nul(&buf), "a3z2x4zzzz");
	EXPECT_TRUE(sb_swap(NULL, 2, 4) == 0);
	sb_free(&buf);
}

TEST_DECL(put)
{
	static STRBUF(buf, 4);
	EXPECT_TRUE(sb_put('1', NULL) == EOF);
	EXPECT_TRUE(sb_nputc('2', NULL, 2) == 0);
	EXPECT_TRUE(sb_puts("abc", NULL) == 0);
	EXPECT_TRUE(sb_nputs("xyz", 2, NULL) == 0);
	EXPECT_TRUE(sb_rputs("ABC", NULL) == 0);

	EXPECT_TRUE(sb_put('1', &buf) == '1');
	EXPECT_TRUE(sb_nputc('2', &buf, 2) == 2);
	EXPECT_TRUE(sb_puts("abc", &buf) == 3);
	EXPECT_TRUE(sb_nputs("xyz", 2, &buf) == 2);
	EXPECT_TRUE(sb_rputs("ABC", &buf) == 3);

	EXPECT_TRUE(sb_put(EOF, &buf) == EOF);
	EXPECT_TRUE(sb_puts(NULL, &buf) == 0);
	EXPECT_TRUE(sb_nputs("xyz", 0, &buf) == 0);
	EXPECT_TRUE(sb_nputs("", 2, &buf) == 0);
	EXPECT_TRUE(sb_nputs(NULL, 2, &buf) == 0);
	EXPECT_TRUE(sb_rputs(NULL, &buf) == 0);

	EXPECT_STR(sb_nul(&buf), "122abcxyCBA");
	sb_free(&buf);

	EXPECT_TRUE(sb_putmem(NULL, "A\0B", 4) == 0);
	EXPECT_TRUE(sb_putmem(&buf, NULL, 0) == 0);
	EXPECT_TRUE(sb_putmem(&buf, "A\0B", 4) == 4);
	EXPECT_TRUE(sb_size(&buf) == 4);
	EXPECT_TRUE(memcmp(sb_nul(&buf), "A\0B", 4) == 0);
	sb_free(&buf);
}

TEST_DECL(ucs)
{
	StrBuf *buf = sb_create(4);
	EXPECT_TRUE(sb_putucs('c', NULL) == 0);
	EXPECT_TRUE(sb_putucs(EOF, buf) == 0);
	EXPECT_TRUE(sb_putucs('a', buf) == 1);
	EXPECT_TRUE(sb_putucs(0177, buf) == 1);
	EXPECT_TRUE(sb_putucs('b', buf) == 1);
	EXPECT_TRUE(sb_putucs(0200, buf) == 2);
	EXPECT_TRUE(sb_putucs(03777, buf) == 2);
	EXPECT_TRUE(sb_putucs('c', buf) == 1);
	EXPECT_TRUE(sb_putucs(04000, buf) == 3);
	EXPECT_TRUE(sb_putucs('d', buf) == 1);
	EXPECT_TRUE(sb_putucs(0200000, buf) == 4);
	EXPECT_TRUE(sb_putucs('e', buf) == 1);
	EXPECT_TRUE(sb_putucs(010000000, buf) == 0);

	EXPECT_STR(sb_nul(buf),
		"a\177b\302\200\337\277c\340\240\200d\360\220\200\200e");
	sb_setpos(buf, 0);
	EXPECT_TRUE(sb_getucs(buf) == 'a');
	EXPECT_TRUE(sb_getucs(buf) == 0177);
	EXPECT_TRUE(sb_getucs(buf) == 'b');
	EXPECT_TRUE(sb_getucs(buf) == 0200);
	EXPECT_TRUE(sb_getucs(buf) == 03777);
	EXPECT_TRUE(sb_getucs(buf) == 'c');
	EXPECT_TRUE(sb_getucs(buf) == 04000);
	EXPECT_TRUE(sb_getucs(buf) == 'd');
	EXPECT_TRUE(sb_getucs(buf) == 0200000);
	EXPECT_TRUE(sb_getucs(buf) == 'e');
	EXPECT_TRUE(sb_getucs(buf) == EOF);

	sb_clean(buf);
	sb_putc(0344, buf); // ä
	sb_putc(' ', buf);
	sb_putc(0337, buf); // ß
	sb_putc(' ', buf);
	sb_putc(0244, buf); // €
	EXPECT_STR(sb_nul(buf), "\344 \337 \244");
	sb_setpos(buf, 0);
	EXPECT_TRUE(sb_getucs(buf) == 0344);
	EXPECT_TRUE(sb_getucs(buf) == ' ');
	EXPECT_TRUE(sb_getucs(buf) == 0337);
	EXPECT_TRUE(sb_getucs(buf) == ' ');
	EXPECT_TRUE(sb_getucs(buf) == 0x20AC);
	EXPECT_TRUE(sb_getucs(buf) == EOF);

	sb_clean(buf);
	EXPECT_TRUE(sb_putmb("äöü€", NULL) == 0);
	EXPECT_TRUE(sb_putmb(NULL, buf) == 0);
	EXPECT_TRUE(sb_putmb("äöü€", buf) == 4);
	EXPECT_TRUE(sb_size(buf) == 9);
	EXPECT_STR(sb_nul(buf), "äöü€");

	sb_destroy(buf);
}

TEST_DECL(printf)
{
	StrBuf *buf = sb_create(2);
	EXPECT_TRUE(sb_printf(NULL, "abc") == 0);
	EXPECT_TRUE(sb_printf(buf, NULL) == 0);
	EXPECT_TRUE(sb_printf(buf, "abcd %d", 42) == 7);
	EXPECT_STR(sb_nul(buf), "abcd 42");
	sb_setpos(buf, 5);
	EXPECT_TRUE(sb_get(NULL) == EOF);
	EXPECT_TRUE(sb_get(buf) == '4');
	EXPECT_TRUE(sb_get(buf) == '2');
	EXPECT_TRUE(sb_get(buf) == EOF);

	sb_destroy(buf);
}

TEST_DECL(xput)
{
	StrBuf *buf = sb_create(2);
	EXPECT_TRUE(sb_xputs("abc", NULL, "") == 0);
	EXPECT_TRUE(sb_xputs(NULL, buf, "") == 0);
	EXPECT_TRUE(sb_xputs("a b", buf, " ") == 4);
	EXPECT_TRUE(sb_xputs("'ä€'", buf, "'") == 9);
	EXPECT_STR(sb_nul(buf), "a\\ b\\'ä€\\'");
	sb_clean(buf);

	EXPECT_TRUE(sb_xputs("a\t\nb :;.", buf, " :.") == 13);
	EXPECT_STR(sb_nul(buf), "a\\t\\nb\\ \\:;\\.");
	sb_clean(buf);

	EXPECT_TRUE(sb_xputs("\b\f\n\r\t\v\\", buf, NULL) == 14);
	EXPECT_STR(sb_nul(buf), "\\b\\f\\n\\r\\t\\v\\\\");
	sb_clean(buf);

	EXPECT_TRUE(sb_xputc('a', NULL, "") == 0);
	EXPECT_TRUE(sb_xputc(EOF, buf, "") == 0);
	EXPECT_TRUE(sb_xputc('a', buf, "") == 1);
	EXPECT_TRUE(sb_xputc(0127, buf, "") == 1);
	EXPECT_TRUE(sb_xputc(0255, buf, "") == 1);
	EXPECT_TRUE(sb_xputc(033, buf, "") == 4);
	EXPECT_STR(sb_nul(buf), "a\127\255\\033");

	sb_destroy(buf);
}

TEST_DECL(read)
{
	int fd;
	TEST_FILE("data", "Testdaten\n");
	fd = open(UnitTest_tmpname("data"), O_RDONLY);
	EXPECT_TRUE(fd >= 0);
	EXPECT_STR(sb_read(fd, NULL), "Testdaten\n");
	close(fd);
}

TEST_SUITE
{
	TEST_RUN(expand);
	TEST_RUN(getmem);
	TEST_RUN(pos);
	TEST_RUN(acquire);
	TEST_RUN(edit);
	TEST_RUN(put);
	TEST_RUN(ucs);
	TEST_RUN(printf);
	TEST_RUN(xput);
	TEST_RUN(read);
}
