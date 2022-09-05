#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>
#include <EFEU/LogConfig.h>
#include <fcntl.h>
#include <unistd.h>

TEST_DECL(expand)
{
	static STRBUF(buf, 32);
	EXPECT(sb_expand(&buf, 100) != NULL);
	EXPECT(buf.size == 128);	// 4 * 32
	EXPECT(buf.blksize == 32);

	EXPECT(sb_expand(&buf, 200) != NULL);
	EXPECT(buf.size == 224);	// 7 * 32
	EXPECT(buf.blksize == 128);

	EXPECT(sb_expand(&buf, 500) != NULL);
	EXPECT(buf.size == 608);	// 224 + 3 * 128
	EXPECT(buf.blksize == 224);

	sb_free(&buf);
	EXPECT(buf.size == 0);
	EXPECT(buf.blksize == 224);

	sb_init(&buf, 0);
	EXPECT(buf.size == 0);
	EXPECT(buf.blksize == 0);

	EXPECT(sb_expand(&buf, 1) != NULL);
	EXPECT(buf.size == 1000);
	EXPECT(buf.blksize == 1000);

	sb_free(&buf);
}

TEST_DECL(nul)
{
	STRBUF(buf, 32);
	EXPECT_STR(sb_nul(NULL), NULL);
	EXPECT_STR(sb_nul(&buf), "");
	EXPECT(buf.pos == 0);
	EXPECT(buf.size == 32);
	sb_puts("abc", &buf);
	EXPECT_STR(sb_nul(&buf), "abc");
	EXPECT(buf.pos == 3);
	EXPECT(buf.size == 32);
	sb_free(&buf);
}

TEST_DECL(cstr)
{
	static STRBUF(buf, 32);
	EXPECT_STR(sb_cstr(NULL), NULL);
	EXPECT_STR(sb_cstr(&buf), "");
	EXPECT(buf.pos == 0);
	EXPECT(buf.size == 32);
	sb_puts("abc", &buf);
	EXPECT_STR(sb_cstr(&buf), "abc");
	EXPECT(buf.pos == 3);
	EXPECT(buf.size == 32);
	sb_free(&buf);
}

TEST_DECL(getmem)
{
	static STRBUF(buf, 32);
	sb_puts("TestDaten", &buf);
	EXPECT_STR(sb_cstr(&buf), "TestDaten");
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
	EXPECT(sb_putstr("abcdef", NULL) == 0);
	EXPECT(sb_putstr("abcdef", &buf) == 7);

	EXPECT(sb_setpos(NULL, 3) == 0);
	EXPECT(sb_setpos(&buf, -1) == 0);
	EXPECT(sb_setpos(&buf, 20) == 7);
	EXPECT(sb_setpos(&buf, 3) == 3);
	sb_puts("123456", &buf);
	EXPECT_STR(sb_cstr(&buf), "abc123456");
	sb_trunc(&buf);
	EXPECT_STR(sb_cstr(&buf), "");
	EXPECT_STR(sb_cstr(NULL), NULL);
}

TEST_DECL(acquire)
{
	StrBuf *buf = sb_acquire();
	sb_release(buf);
	EXPECT(sb_acquire() == buf);

	sb_putstr("abc", buf);
	char *p = sb_cpyrelease(buf);
	EXPECT_STR(p, "abc");
	memfree(p);

	StrBuf *tab[5];
	int i;
	
	for (i = 0; i < 5; i++)
		tab[i] = sb_acquire();
	
	for (i = 0; i < 5; i++)
		sb_release(tab[i]);
}

TEST_DECL(edit)
{
	static STRBUF(buf, 4);
	EXPECT(sb_insert(NULL, 'a', 3) == NULL); 
	EXPECT(memcmp(sb_insert(&buf, 'a', 3), "aaa", 3) == 0); 
	sb_setpos(&buf, 2);
	EXPECT(memcmp(sb_insert(&buf, 'x', 2), "xx", 2) == 0); 
	sb_end(&buf);
	EXPECT_STR(sb_cstr(&buf), "aaxxa");
	EXPECT(sb_delete(NULL, 1) == NULL); 
	EXPECT(sb_delete(&buf, 1) == NULL); 
	sb_setpos(&buf, 1);
	EXPECT(memcmp(sb_delete(&buf, 2), "ax", 2) == 0); 
	sb_end(&buf);
	EXPECT_STR(sb_cstr(&buf), "axa");
	sb_puts("12345", &buf);
	EXPECT(sb_move(&buf, 1, 5, 2) == 2);
	EXPECT_STR(sb_cstr(&buf), "a34xa125");
	EXPECT(sb_move(&buf, 4, 6, 0) == 2);
	EXPECT_STR(sb_cstr(&buf), "a34x25a1");
	sb_nputc('z', &buf, 1000);
	EXPECT(sb_size(&buf) == 1008);
	EXPECT(sb_move(&buf, 5, 8, 0) == 1000);
	sb_setpos(&buf, 10);
	EXPECT_STR(sb_cstr(&buf), "a34x2zzzzz");
	EXPECT(sb_swap(&buf, 2, 4) == 4);
	EXPECT_STR(sb_cstr(&buf), "a3z2x4zzzz");
	EXPECT(sb_swap(NULL, 2, 4) == 0);
	sb_free(&buf);
}

TEST_DECL(ungetc)
{
	static STRBUF(buf, 4);
	EXPECT(sb_puts("1234567890", &buf) == 10);
	sb_setpos(&buf, 3);
	EXPECT(sb_ungetc('a', &buf) == 'a');
	EXPECT(sb_ungetc('b', &buf) == 'b');
	EXPECT(sb_ungetc('c', &buf) == 'c');
	EXPECT(sb_ungetc('d', &buf) == EOF);
	sb_end(&buf);
	EXPECT_STR(sb_cstr(&buf), "cba4567890");
	sb_free(&buf);
}

TEST_DECL(put)
{
	static STRBUF(buf, 4);
	EXPECT(sb_put('1', NULL) == EOF);
	EXPECT(sb_nputc('2', NULL, 2) == 0);
	EXPECT(sb_puts("abc", NULL) == 0);
	EXPECT(sb_nputs("xyz", 2, NULL) == 0);
	EXPECT(sb_rputs("ABC", NULL) == 0);

	EXPECT(sb_put('1', &buf) == '1');
	EXPECT(sb_nputc('2', &buf, 2) == 2);
	EXPECT(sb_puts("abc", &buf) == 3);
	EXPECT(sb_nputs("xyz", 2, &buf) == 2);
	EXPECT(sb_rputs("ABC", &buf) == 3);

	EXPECT(sb_put(EOF, &buf) == EOF);
	EXPECT(sb_puts(NULL, &buf) == 0);
	EXPECT(sb_nputs("xyz", 0, &buf) == 0);
	EXPECT(sb_nputs("", 2, &buf) == 0);
	EXPECT(sb_nputs(NULL, 2, &buf) == 0);
	EXPECT(sb_rputs(NULL, &buf) == 0);

	EXPECT_STR(sb_cstr(&buf), "122abcxyCBA");
	sb_free(&buf);

	EXPECT(sb_putmem(NULL, "A\0B", 4) == 0);
	EXPECT(sb_putmem(&buf, NULL, 0) == 0);
	EXPECT(sb_putmem(&buf, "A\0B", 4) == 4);
	EXPECT(sb_size(&buf) == 4);
	EXPECT(memcmp(sb_cstr(&buf), "A\0B", 4) == 0);
	sb_free(&buf);
}

TEST_DECL(ucs)
{
	StrBuf *buf = sb_create(4);
	EXPECT(sb_putucs('c', NULL) == 0);
	EXPECT(sb_putucs(EOF, buf) == 0);
	EXPECT(sb_putucs('a', buf) == 1);
	EXPECT(sb_putucs(0177, buf) == 1);
	EXPECT(sb_putucs('b', buf) == 1);
	EXPECT(sb_putucs(0200, buf) == 2);
	EXPECT(sb_putucs(03777, buf) == 2);
	EXPECT(sb_putucs('c', buf) == 1);
	EXPECT(sb_putucs(04000, buf) == 3);
	EXPECT(sb_putucs('d', buf) == 1);
	EXPECT(sb_putucs(0200000, buf) == 4);
	EXPECT(sb_putucs('e', buf) == 1);
	EXPECT(sb_putucs(010000000, buf) == 0);

	EXPECT_STR(sb_cstr(buf),
		"a\177b\302\200\337\277c\340\240\200d\360\220\200\200e");
	sb_setpos(buf, 0);
	EXPECT(sb_getucs(buf) == 'a');
	EXPECT(sb_getucs(buf) == 0177);
	EXPECT(sb_getucs(buf) == 'b');
	EXPECT(sb_getucs(buf) == 0200);
	EXPECT(sb_getucs(buf) == 03777);
	EXPECT(sb_getucs(buf) == 'c');
	EXPECT(sb_getucs(buf) == 04000);
	EXPECT(sb_getucs(buf) == 'd');
	EXPECT(sb_getucs(buf) == 0200000);
	EXPECT(sb_getucs(buf) == 'e');
	EXPECT(sb_getucs(buf) == EOF);

	sb_clean(buf);
	sb_putc(0344, buf); // ä
	sb_putc(' ', buf);
	sb_putc(0337, buf); // ß
	sb_putc(' ', buf);
	sb_putc(0244, buf); // €
	EXPECT_STR(sb_cstr(buf), "\344 \337 \244");
	sb_setpos(buf, 0);
	EXPECT(sb_getucs(buf) == 0344);
	EXPECT(sb_getucs(buf) == ' ');
	EXPECT(sb_getucs(buf) == 0337);
	EXPECT(sb_getucs(buf) == ' ');
	EXPECT(sb_getucs(buf) == 0x20AC);
	EXPECT(sb_getucs(buf) == EOF);

	sb_clean(buf);
	EXPECT(sb_putmb("äöü€", NULL) == 0);
	EXPECT(sb_putmb(NULL, buf) == 0);
	EXPECT(sb_putmb("äöü€", buf) == 4);
	EXPECT(sb_size(buf) == 9);
	EXPECT_STR(sb_cstr(buf), "äöü€");

	sb_destroy(buf);
}

TEST_DECL(printf)
{
	StrBuf *buf = sb_create(2);
	EXPECT(sb_printf(NULL, "abc") == 0);
	EXPECT(sb_printf(buf, NULL) == 0);
	EXPECT(sb_printf(buf, "abcd %d", 42) == 7);
	EXPECT_STR(sb_cstr(buf), "abcd 42");
	sb_setpos(buf, 5);
	EXPECT(sb_get(NULL) == EOF);
	EXPECT(sb_get(buf) == '4');
	EXPECT(sb_get(buf) == '2');
	EXPECT(sb_get(buf) == EOF);

	sb_destroy(buf);
}

TEST_DECL(xput)
{
	StrBuf *buf = sb_create(2);
	EXPECT(sb_xputs("abc", NULL, "") == 0);
	EXPECT(sb_xputs(NULL, buf, "") == 0);
	EXPECT(sb_xputs("a b", buf, " ") == 4);
	EXPECT(sb_xputs("'ä€'", buf, "'") == 9);
	EXPECT_STR(sb_cstr(buf), "a\\ b\\'ä€\\'");
	sb_clean(buf);

	EXPECT(sb_xputs("a\t\nb :;.", buf, " :.") == 13);
	EXPECT_STR(sb_cstr(buf), "a\\t\\nb\\ \\:;\\.");
	sb_clean(buf);

	EXPECT(sb_xputs("\b\f\n\r\t\v\\", buf, NULL) == 14);
	EXPECT_STR(sb_cstr(buf), "\\b\\f\\n\\r\\t\\v\\\\");
	sb_clean(buf);

	EXPECT(sb_xputc('a', NULL, "") == 0);
	EXPECT(sb_xputc(EOF, buf, "") == 0);
	EXPECT(sb_xputc('a', buf, "") == 1);
	EXPECT(sb_xputc(0127, buf, "") == 1);
	EXPECT(sb_xputc(0255, buf, "") == 1);
	EXPECT(sb_xputc(033, buf, "") == 4);
	EXPECT_STR(sb_cstr(buf), "a\127\255\\033");

	sb_destroy(buf);
}

TEST_DECL(read)
{
	int fd;
	TEST_FILE("data", "Testdaten\n");
	fd = open(UnitTest_tmpname("data"), O_RDONLY);
	EXPECT(fd >= 0);
	EXPECT_STR(sb_read(fd, NULL), "Testdaten\n");
	close(fd);
}

TEST_SUITE
{
	TEST_RUN(expand);
	TEST_RUN(nul);
	TEST_RUN(cstr);
	TEST_RUN(getmem);
	TEST_RUN(pos);
	TEST_RUN(acquire);
	TEST_RUN(edit);
	TEST_RUN(ungetc);
	TEST_RUN(put);
	TEST_RUN(ucs);
	TEST_RUN(printf);
	TEST_RUN(xput);
	TEST_RUN(read);
}
