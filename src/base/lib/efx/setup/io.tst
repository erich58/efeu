#!/usr/bin/env .build/script.tst

TEST(null)
{
	TEST_EXPR(IO io = NULL);
	EXPECT_STR(io.gets(EOF), NULL);
	EXPECT_REPR(io, "NULL");
}

TEST(string)
{
	EXPECT(IO io = "abc");
	EXPECT_STR(io.gets(EOF), "abc");
	EXPECT_REPR(io, string(IO: "abc":3));
}

TEST(file)
{
	EXPECT(chdir(UnitTest::tmp()));
	EXPECT(IO io = open("data", "w"));
	EXPECT_REPR(io, "IO: data");
	EXPECT(io.puts("content\n") == 8);
	close(io);

	EXPECT(chdir("/"));
	EXPECT_CONTENT("data", "content\n");
	EXPECT(IO io = findopen(UnitTest::tmp(), "data"));
	EXPECT_STR(io.gets(EOF), "content\n");
}

TEST(popen)
{
	EXPECT(IO io = popen("echo Test", "r"));
	EXPECT_STR(io.gets(EOF), "Test\n");
	EXPECT_REPR(io, "IO: <!echo>");
}

TEST(tmpfile)
{
	EXPECT(IO io = tmpfile());
	EXPECT(io.puts("data\n") == 5);
	EXPECT(io.rewind());
	EXPECT_STR(io.gets(EOF), "data\n");
	EXPECT_REPR(io, "IO: <tmpfile>");
}

TEST(tmpbuf)
{
	EXPECT(IO io = tmpbuf());
	EXPECT(io.puts("data\n") == 5);
	EXPECT(io.rewind());
	EXPECT_STR(io.gets(EOF), "data\n");
	EXPECT_REPR(io, "IO: <tmpbuf>");
}

TEST(bigbuf)
{
	EXPECT(IO io = bigbuf(1000));
	EXPECT(io.puts("data\n") == 5);
	EXPECT(io.rewind());
	EXPECT_STR(io.gets(EOF), "data\n");
	EXPECT_REPR(io, "IO: bigbuf() 0+0 0+5");
}

TEST(interactive)
{
	UnitTest::stdin("input", "data\n");
	EXPECT(IO io = iaopen("> "));
	EXPECT_STR(io.gets(EOF), "data\n");
	UnitTest::stdin();
}

TEST(iofilter)
{
	EXPECT(IO io = ":de:deutsch:*:german");
	EXPECT_STR(langfilter(io, "de").gets(EOF), "deutsch");
	EXPECT(io.rewind());
	EXPECT_STR(langfilter(io, "C").gets(EOF), "german");
	EXPECT_REPR(lnum(tmpbuf()), "IO: <tmpbuf>:1");
	EXPECT(linemark(OUT(), "%d ", "$").puts("a\nb\nc\n") == 6);
	EXPECT_OUT("1 a$\n2 b$\n3 c$\n");
	EXPECT(out_utf8(OUT()).puts("\344") == 1);
	EXPECT_OUT("\303\244");
	EXPECT(out_latin9(OUT()).puts("\303\244") == 2);
	EXPECT_OUT("\344");
}

TEST(indent)
{
	IO out = indent(OUT(), '+', 2);
	out.puts("line 1\n");
	out.submode(true);
	out.puts("line 2\n");
	out.submode(true);
	out.puts("line 3\n");
	out.submode(false);
	out.puts("line 4\n");
	out.submode(false);
	out.puts("line 5\n");
	EXPECT_OUT(string !
line 1
++line 2
++++line 3
++line 4
line 5
!);
}

TEST(push_pop)
{
	IO io = "abc";
	EXPECT(io.getc() == 'a');
	io.push((IO) "xyz");
	EXPECT(io.getc() == 'x');
	EXPECT(io.getc() == 'y');
	EXPECT(io.getc() == 'z');
	EXPECT(io.ungetc('z'));
	IO sub = io.pop();
	EXPECT(sub.getc() == 'z');
	EXPECT(io.getc() == 'b');
	EXPECT(io.getc() == 'c');
	EXPECT(io.getc() == EOF);
}

TEST(seek)
{
	IO io = "abc";
	unsigned pos = 0;
	EXPECT(io.getc() == 'a');
	EXPECT(io.getpos(pos));
	EXPECT(pos == 1);
	EXPECT(io.setpos(2));
	EXPECT(io.getc() == 'c');
	rewind(io);
	EXPECT(io.getc() == 'a');
	close(io);
	EXPECT(io == NULL);
}

TEST(divide)
{
	IO base = OUT();
	IO sub = base.divide();
	base.puts("b1");
	sub.puts("s1");
	base.puts("b2");
	sub.puts("s2");
	sub.flush();
	base.puts("b3");
	sub.puts("s3");
	close(sub);
	close(base);
	EXPECT_OUT("b1b2s1s2b3s3");
}

TEST(diropen)
{
	IO io = diropen(UnitTest::tmp(), "main.txt");
	EXPECT(io.puts("Start\n"));
	EXPECT(io.newpart("chapter.txt", "include $1\n") == 0); 
	EXPECT(io.puts("Chapter\n"));
	EXPECT(io.newpart("section.txt", "include $1\n") == 0); 
	EXPECT(io.puts("Section\n"));
	EXPECT(io.endpart() == 0); 
	EXPECT(io.puts("Data\n"));
	EXPECT(io.endpart() == 0); 
	EXPECT(io.puts("End\n"));
	io.flush();
	close(io);

	EXPECT_CONTENT("main.txt", "Start\ninclude chapter.txt\nEnd\n");
	EXPECT_CONTENT("chapter.txt", "Chapter\ninclude section.txt\nData\n");
	EXPECT_CONTENT("section.txt", "Section\n");
}

TEST(getline)
{
	ApplPath = UnitTest::tmp();
	TEST_FILE("efmain.msg", "88	$1: missing L-value\n");

	IO in = "20 data rest of line\n #comment\n  10\n\n\n\tdata rest\n";
	int n;
	str a, b;
	EXPECT(getline(in, n, a, b));
	EXPECT(n == 20);
	EXPECT_STR(a, "data");
	EXPECT_STR(b, "rest of line");
	EXPECT(getline(in, n, a, b));
	EXPECT(n == 10);
	EXPECT_STR(a, "");
	EXPECT_STR(b, "");
	EXPECT(getline(in, n, a, b));
	EXPECT(n == 0);
	EXPECT_STR(a, "rest");
	EXPECT_STR(b, "");
	EXPECT(getline(in, n, a, b) == false);
	in.rewind();
	UnitTest::stderr("stderr");
	EXPECT(getline(in, n, "", b));
	UnitTest::stderr();
	EXPECT_CONTENT("stderr", "MSG getline: missing L-value\n");
	EXPECT(getline(in));
}

TEST(get)
{
	EXPECT(IO in = "\303\244 \t\na");
	EXPECT(in.peek() == 0303);
	EXPECT(in.getwc() == 0344);
	EXPECT(in.eat("%s") == 'a');
	EXPECT(in.getc() == 'a');
}

TEST(putwc)
{
	EXPECT(IO out = OUT());
	out.ucswmode("latin9");
	out.putwc(0344);
	out.ucswmode("utf8");
	out.putwc('<');
	out.putwc(0344);
	out.putwc('>');
	out.ucswmode("xml");
	out.putc('<');
	out.putwc('<');
	out.putwc(0344);
	out.putc('>');
	out.ucswmode("ascii");
	out.putwc(0344);
//	out.putwc(L'€');
//	EXPECT_OUT("\344<\303\244><&lt;&\043228;>\344?"); // TODO: # in Macro ignored
}

TEST(puts)
{
	EXPECT(IO out = OUT());
	EXPECT(out.puts("a\tb ") == 4);
	EXPECT(out.xputs("a\tb ") == 5);
	EXPECT(out.mputs("a b", " ") == 4);
	EXPECT_OUT("a\tb a\\tb a\\ b");

}

TEST(gets)
{
	EXPECT(IO in = "a\\t\\,b,c");
	EXPECT_STR(in.gets(), "a\\t\\,b,c"); 
	EXPECT(in.rewind());
	EXPECT_STR(in.gets(','), "a\\t\\"); 
	EXPECT(in.rewind());
	EXPECT_STR(in.xgets(), "a\t\\,b,c"); 
	EXPECT(in.rewind());
	EXPECT_STR(in.xgets(","), "a\t,b"); 
	EXPECT(in.rewind());
	EXPECT_STR(in.mgets(), "a\\t\\,b,c"); 
	EXPECT(in.rewind());
	EXPECT_STR(in.mgets(","), "a\\t,b"); 
	EXPECT(IO in = "\\t");
	EXPECT(in.xgetc() == '\t');

	EXPECT(IO in = "\\a\\\n \tb");
	EXPECT(in.mgetc(false) == '\\');
	EXPECT(in.mgetc(false) == 'a');
	EXPECT(in.mgetc(false) == ' ');

	EXPECT(IO in = "\\a\\\n \t\\\n b");
	EXPECT(in.mgetc() == '\\');
	EXPECT(in.mgetc() == 'a');
	EXPECT(in.mgetc() == 'b');

	EXPECT(IO in = "abc");
	EXPECT_STR(ngets(in, -1), NULL);
	EXPECT_STR(ngets(in, 2), "ab");
	EXPECT_STR(ngets(in, 2), "c");
	EXPECT_STR(ngets(in, 2), NULL);
}

TEST(scanline)
{
	EXPECT(IO in = string(3 a\ b 'c' "d"));
	EXPECT_REPR(scanline(in), string({"3", "a b", "c", "d"}));
}

TEST(copy)
{
	EXPECT(copy("abc\n", OUT()) == 4);
	EXPECT_OUT("abc\n");
}

TEST(filter)
{
	EXPECT_STR(filter(NULL, "data"), "data");
	EXPECT_STR(filter("echo Test"), "Test");
	EXPECT_STR(filter("nl -ba -nln -w1", "data\nline"),
		"1\tdata\n2\tline");
}

TEST(plain)
{
	EXPECT(int x = 0x10203040);
	EXPECT(IO io = tmpbuf());
	EXPECT(write(io, x) == 4);
	EXPECT(io.rewind());
	int y;
	EXPECT(read(io, y) == 4);
	EXPECT(y == 0x10203040);
	EXPECT(io.rewind());
	EXPECT(io.getc() == 0x10);
	EXPECT(io.getc() == 0x20);
	EXPECT(io.getc() == 0x30);
	EXPECT(io.getc() == 0x40);
	EXPECT(io.rewind());
	EXPECT(print(io, x));
	EXPECT(io.putc('\n'));
	EXPECT(io.rewind());
	EXPECT_STR(io.gets(), (str) 0x10203040);
}

/* coverage only, tested in efm */
ionull.protect(true);
