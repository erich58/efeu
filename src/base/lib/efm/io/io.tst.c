#include <EFEU/UnitTest.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/Debug.h>
#include <EFEU/mstring.h>

#include "utest/m2c.tst.h"

TEST_DECL(logging)
{
	LogConfig("@:");
	TEST_EXPR(LogConfig(UnitTest_tpl("IO.err=@/io.err")));
	TEST_EXPR(LogConfig(UnitTest_tpl("IO.note=@/io.note")));
}

TEST_DECL(null)
{
	EXPECT_STR(rd_ident(NULL), NULL);
	EXPECT_STR(M2C(io_xident(NULL, NULL)), NULL);
	EXPECT_STR(M2C(io_xident(NULL, ":%*:")), "::");

	EXPECT(io_putc('a', NULL) == 0);
	EXPECT(io_nputc('a', NULL, 5) == 0);
	EXPECT(io_puts("abc", NULL) == 0);
	EXPECT(io_err(NULL) == 0);
	EXPECT(io_rewind(NULL) == EOF);

	EXPECT(io_getc(NULL) == EOF);
	EXPECT(io_peek(NULL) == EOF);
	EXPECT(io_ungetc('a', NULL) == 0);
	EXPECT(io_getc(NULL) == EOF);

	char buf[8];
	EXPECT(io_gets(buf, sizeof buf, NULL) == NULL);
}

TEST_DECL(plain)
{
	IO *io = io_alloc();
	EXPECT_STR(M2C(rd_ident(io)), "<io>");
	EXPECT_STR(M2C(io_xident(io, NULL)), "<io>");
	EXPECT_STR(M2C(io_xident(io, "a%*b:%d%%.%", 42)), "a<io>b:42%.");
	EXPECT_STR(M2C(io_xident(io, "%m %s", mstrcpy("a"), "x")), "a x");

	EXPECT(io_putc('a', io) == 0);
	EXPECT(io_nputc('a', io, 5) == 0);
	EXPECT(io_puts("abc", io) == 0);
	EXPECT(io_err(io) == 0);
	EXPECT(io_rewind(io) == EOF);

	EXPECT(io_getc(io) == EOF);
	EXPECT(io_peek(io) == EOF);
	EXPECT(io_ungetc('a', io) == 1);
	EXPECT(io_getc(io) == 'a');

	char buf[8];
	EXPECT_STR(io_gets(buf, sizeof buf, io), NULL);

	EXPECT(io_ungetc('1', io) == 1);
	EXPECT(io_ungetc('2', io) == 1);
	EXPECT(io_ungetc('3', io) == 1);
	EXPECT(io_ungetc('4', io) == 1);
	EXPECT(io_ungetc('5', io) == 1);
	EXPECT(io_ungetc('6', io) == 1);
	EXPECT(io_ungetc('7', io) == 0);
	EXPECT_STR(io_gets(buf, sizeof buf, io), "654321");

	TEST_FILE("io.note",NULL);
	io_note(io, "$0: NOTE $1 $2", "sd", "abc", 5);
	EXPECT_CONTENT("io.note","<io>: NOTE abc 5\n");

	TEST_FILE("io.err",NULL);

	TEST_FORK
	{
		io_error(io, "$0: ERROR $1 $2", "sd", "abc", 5);
		exit(0);
	}

	CHILD_EXIT(1);
	EXPECT_CONTENT("io.err","<io>: ERROR abc 5\n");

	EXPECT(io_close(io) == EOF);
}

static const char *prompt = "P1";
static int depth = 0;
static int mark = 0;

static int test_ctrl (IO *io, int req, va_list list)
{
	const char **ptr;

	switch (req)
	{
	case IO_LINEMARK:
		mark = 1;
		return 0;
	case IO_PROTECT:
	case IO_SUBMODE:
		if	(va_arg(list, int))	depth++;
		else				depth--;

		return depth;
	case IO_PROMPT:
		if	((ptr = va_arg(list, const char **)) != NULL)
		{
			const char *save = prompt;
			prompt = *ptr;
			*ptr = save;
			return 0;
		}
		
		return EOF;
	case IO_UNGETC:
		va_arg(list, int);
		return 0;
	case IO_CLOSE:
		return 42;
	default:
		return EOF;
	}
}

TEST_DECL(ctrl)
{
	IO *io;
	const char *p;
	va_list list;

	EXPECT((io = io_alloc()) != NULL);
	io->ctrl = test_ctrl;
	EXPECT(io_vctrl(io, 0, list) == EOF);

	io_linemark(io);
	EXPECT(mark == 1);
	io_submode(io, 1);
	EXPECT(depth == 1);
	io_protect(io, 1);
	EXPECT(depth == 2);
	io_protect(io, 0);
	io_submode(io, 0);
	EXPECT(depth == 0);

	EXPECT_STR((p = io_prompt(io, "P2")), "P1");
	EXPECT_STR(io_prompt(io, p), "P2");
	EXPECT_STR(prompt, "P1");

	EXPECT(io_ungetc('x', io) == 1);
	EXPECT(io_close(io) == 42);
}

TEST_SUITE
{
	TEST_RUN(logging);
	TEST_RUN(null);
	TEST_RUN(plain);
	TEST_RUN(ctrl);
}
