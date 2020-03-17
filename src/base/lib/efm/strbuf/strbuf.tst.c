#include <EFEU/UnitTest.h>
#include <EFEU/strbuf.h>

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

TEST_SUITE
{
	TEST_RUN(expand);
	TEST_RUN(getmem);
	TEST_RUN(pos);
	TEST_RUN(acquire);
	TEST_RUN(edit);
}
