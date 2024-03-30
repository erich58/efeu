#include <EFEU/UnitTest.h>
#include "efeudoc.h"

TEST_SUITE
{
	IO *io;
	
	EXPECT_PTR(io = io_cstr(" \t x"));
	EXPECT_INT(DocSkipSpace(io, 0), 'x');
	EXPECT_INT(io_getc(io), 'x');
	io_rewind(io);
	EXPECT_INT(DocSkipSpace(io, 1), 'x');
	EXPECT_INT(io_getc(io), 'x');
	io_close(io);

	EXPECT_PTR(io = io_cstr(" \t /* ccc */ \nx"));
	EXPECT_INT(DocSkipSpace(io, 0), '\n');
	EXPECT_INT(io_getc(io), '\n');
	io_rewind(io);
	EXPECT_INT(DocSkipSpace(io, 1), '\n');
	EXPECT_INT(io_getc(io), 'x');
	io_rewind(io);
	EXPECT_INT(DocSkipWhite(io), 'x');
	EXPECT_INT(io_getc(io), 'x');
	io_close(io);

	EXPECT_PTR(io = io_cstr(" \t // ccc\n x"));
	EXPECT_INT(DocSkipSpace(io, 0), '\n');
	EXPECT_INT(io_getc(io), '\n');
	io_rewind(io);
	EXPECT_INT(DocSkipSpace(io, 1), '\n');
	EXPECT_INT(io_getc(io), ' ');
	io_rewind(io);
	EXPECT_INT(DocSkipWhite(io), 'x');
	EXPECT_INT(io_getc(io), 'x');
	io_close(io);
}
