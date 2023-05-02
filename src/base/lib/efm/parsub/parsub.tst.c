#include <EFEU/UnitTest.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <errno.h>

#include "utest/mstr.tst.h"
#include "utest/in.tst.h"

static STRBUF(sbuf, 64);
static IO *io;

static char *output()
{
	sb_putc(0, &sbuf);
	sb_setpos(&sbuf, 0);
	sb_sync(&sbuf);
	return sb_ptr(&sbuf);
}

#define	EXPECT_OUTPUT(b) \
	EXPECT_STR(output(), b)

static int psub_test(IO *in, IO *out, void *par)
{
	return io_puts(par, out);
}

TEST_DECL(setup)
{
	SetApplPath(UnitTest_tmp(NULL));
	TEST_FILE("efm.msg",
		"61	NOTE 61: arg1=$1.\n"
		"pnoarg	NOTE pnoarg: fmt=$0 arg=$1.\n"
		"pilkey	NOTE pilkey: fmt=$0 arg=$1.\n"
	);

	io = io_strbuf(&sbuf);
	psubfunc('!', psub_test, "[ID]");
}

TEST_DECL(argl)
{
	ArgList *argl;
	EXPECT_PTR(argl = arg_create());
	EXPECT_MSTR(rd_ident(argl), "NULL");
	EXPECT_STR(arg_get(NULL, 0), NULL);
	EXPECT_STR(arg_get(argl, 0), NULL);

	arg_cadd(argl, "str");
	arg_madd(argl, mstrcpy("mstr"));
	arg_printf(argl, "%d", 42);
	EXPECT_STR(arg_get(argl, 0), "str");
	EXPECT_STR(arg_get(argl, 1), "mstr");
	EXPECT_STR(arg_get(argl, -1), "42");
	EXPECT_MSTR(rd_ident(argl), "str \"mstr\" \"42\"");
	EXPECT(arg_test(argl, 2) == 1);
	arg_undo(argl);
	EXPECT(arg_test(argl, 2) == 0);
	arg_mset(argl, -1, "a");
	arg_mset(argl, 3, "b");
	EXPECT_MSTR(rd_ident(argl), "str \"a\" NULL \"b\"");
	arg_cset(argl, 100, NULL); // coverage für realloc
	EXPECT(rd_deref(argl) == NULL);
}

TEST_DECL(psubarg)
{
	EXPECT(io_psubarg(io, "'$0' $*", "nsmp", "a", mstrcpy("x"), (void *) 0xFF) == 11);
	EXPECT_OUTPUT("'' a x 0xff");
	EXPECT(io_psubarg(io, "$*", "nduzU", 1, 2u, (size_t) 3, (uint64_t) 4)== 7);
	EXPECT_OUTPUT("1 2 3 4");
	EXPECT(io_psubarg(io, "$*", "nxXc", 1u, (uint64_t) 2, 'c') == 11);
	EXPECT_OUTPUT("0x1 0x2 'c'");
	EXPECT(io_psubarg(io, "$# $*", "n*", "a", "b", NULL) == 5);
	EXPECT_OUTPUT("2 a b");
	EXPECT(io_psubarg(io, "$*", "n%", "|%5d%%|", 42) == 8);
	EXPECT_OUTPUT("|   42%|");
	errno = EACCES;
	EXPECT(io_psubarg(io, "$0", "e"));
	EXPECT_OUTPUT(strerror(errno));

	UnitTest_stderr("stderr");
	EXPECT(io_psubarg(io, "$*", "nw") == 0);
	EXPECT_OUTPUT("");
	EXPECT_CONTENT("stderr", "MSG NOTE 61: arg1='w'.\n");

	EXPECT(io_psubarg(io, "x", NULL) == 1);
	EXPECT_OUTPUT("x");
	EXPECT(io_psubarg(io, NULL, NULL) == 0);
	EXPECT_OUTPUT("");

	EXPECT_MSTR(mpsubarg("$*", "nd", 42), "42");
	EXPECT_MSTR(mpsubarg(NULL, NULL), NULL);
}

TEST_DECL(psubvec)
{
	char *argv[] = { "a0", "a1", "a2" };
	EXPECT(io_psubvec(io, NULL, 3, argv) == 0);
	EXPECT_OUTPUT("");
	EXPECT(io_psubvec(io, "$0 $1 $2", 3, argv) == 8);
	EXPECT_OUTPUT("a0 a1 a2");

	EXPECT_MSTR(mpcopy(IN("$0 $2"), 0, 3, argv), "a0 a2");
	EXPECT_STR(psubexpand(NULL, NULL, 0, NULL), "");
}

TEST_DECL(expand)
{
	char *argv[] = { "a0", "a1", "a2", "a3", "a4", "a5" };

	EXPECT_MSTR(mpsubvec("$!", 6, argv), "[ID]");
	EXPECT_MSTR(mpsubvec("|$[3:2]|", 6, argv), "|a3 a2|");
	EXPECT_MSTR(mpsubvec("|$[1:3;<%s>]|", 6, argv), "|<a1> <a2> <a3>|");
	EXPECT_MSTR(mpsubvec("|$[-1:-2<%s>]|", 6, argv), "|<a5> <a4>|");
	EXPECT_MSTR(mpsubvec("|$[1;%s]|$[1\\;%s]|", 6, argv), "|a1|;a1|");
	EXPECT_MSTR(mpsubvec("\"$1\"", 6, argv), "\"a1\"");
	EXPECT_MSTR(mpsubvec("|$[1\\[%s\\]]|", 6, argv), "|[a1]|");
	EXPECT_MSTR(mpsubvec("abc$[1", 6, argv), "abc");
	EXPECT_MSTR(mpsubvec("abc$$", 6, argv), "abc$");
	EXPECT_MSTR(mpsubvec(NULL, 6, argv), NULL);

	EXPECT_MSTR(mpsubarg("|$[1;%4d]|", "nd", -42), "| -42|");
	EXPECT_MSTR(mpsubarg("|$[1;%hx]|", "nd", -1), "|ffff|");
	EXPECT_MSTR(mpsubarg("|$[1;%c]|", "ns", "abc"), "|a|");
	EXPECT_MSTR(mpsubarg("|$[1;%#c]|", "ns", "abc"), "|'a'|");
	EXPECT_MSTR(mpsubarg("|$[1;%5.2f]|", "ns", "3.5"), "| 3.50|");

	PRINT_LABEL("error handling");
	UnitTest_stderr("stderr");
	EXPECT_MSTR(mpsubvec("|$1|", 0, argv), "||");
	EXPECT_CONTENT("stderr", "MSG NOTE pnoarg: fmt=\"|$1|\":3 arg=1.\n");
	UnitTest_stderr("stderr");
	EXPECT_MSTR(mpsubvec("|$[1;%w]|", 6, argv), "|a1|");
	EXPECT_CONTENT("stderr", "MSG NOTE pilkey: fmt=\"|$[1;%w]|\":8 arg='w'.\n");
}

TEST_DECL(psubfilter)
{
	IO *io;
	ArgList *argl;
	TEST_FILE("input", "$0|\"$1\"|$2|$3\n");
	EXPECT_PTR(argl = arg_create());
	arg_cadd(argl, "a");
	arg_cadd(argl, "\"\\x\"");
	arg_cadd(argl, NULL);
	arg_cadd(argl, "10");
	EXPECT_PTR(io = io_fopen(UnitTest_tmp("input"), "r"));
	EXPECT_PTR(io = psubfilter(io, argl));
	EXPECT_STR(io_mgets(io, NULL), "a|\"\\\"\\\\x\\\"\"||10\n");
	EXPECT(io_rewind(io) == 0);
	EXPECT(io_close(io) == 0);

	io = psubfilter(io_alloc(), NULL);
	EXPECT(io_rewind(io) == EOF);
	EXPECT(io_close(io) == EOF);
}

TEST_SUITE
{
	TEST_RUN(setup);
	TEST_RUN(argl);
	TEST_RUN(psubarg);
	TEST_RUN(psubvec);
	TEST_RUN(expand);
	TEST_RUN(psubfilter);
	CLEANUP_IN;
	CLEANUP_MSTR;
}
