#!/usr/bin/env .build/script.tst

StrBuf buf = "abc";
fprint(OUT(), buf);
MATCH_OUT("strbuf.* = \"abc\"");
buf.free();

/* unsupported ! */
EXPECT(write(OUT(), buf) == 0);
EXPECT(print(OUT(), buf) == 0);
EXPECT(read((IO)"x", buf) == 0);

TEST_FILE("stdin", "Testtext\n");
UnitTest::stdin("stdin");
EXPECT(buf.read(0));
EXPECT_STR(buf, "Testtext\n");
EXPECT(buf.pos() == 9);
EXPECT(buf[1] == 'e');
EXPECT(buf[10] == 0);
UnitTest::stdin();
EXPECT(buf.seek(3) == 3);
EXPECT(buf.getc() == 't');
EXPECT(buf.putc('\n') == '\n');
TEST_EXPR(buf.sync());
EXPECT_STR(buf, "Test\n");
EXPECT(buf.seek(1000) == 5);
EXPECT(buf.seek(0) == 0);
EXPECT(buf.pos() == 0);
EXPECT(buf.putc('ä') == 0344);
EXPECT(buf.putw(L'ä') == 2);
EXPECT_STR(buf, "\344\303\244");
buf.sync();
EXPECT(buf.seek(0) == 0);
EXPECT(buf.getw() == 0344);
EXPECT(buf.getw() == 0344);
EXPECT(buf.getw() == EOF);
EXPECT(buf.seek(0) == 0);
EXPECT(buf.printf("%d %.2f ", 42, 3.5) == 8);
EXPECT(buf.puts("abc") == 3);
buf.sync();
EXPECT_STR(buf, "42 3.50 abc");
EXPECT(buf.seek(2) == 2);
EXPECT(buf.delete(4));
EXPECT_STR(buf, "420 abc");
TEST_EXPR(buf.move(1, 4, 2));
EXPECT_STR(buf, "4ab20 c");
EXPECT(buf.seek(2) == 2);
TEST_EXPR(buf.insert('x', 3));
EXPECT_STR(buf, "4axxxb20 c");
EXPECT(buf.seek(5) == 5);
EXPECT(fgetc((IO) buf) == 'b');
