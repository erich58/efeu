#include <EFEU/UnitTest.h>
#include <EFEU/pconfig.h>
#include <EFEU/CmdPar.h>
#include <EFEU/preproc.h>

TEST_SUITE
{
	EXPECT_STR(CmdPar_getval(NULL, "Ident", NULL), NULL);
	EXPECT_STR(CmdPar_getval(NULL, "Copyright", NULL), NULL);

	IO *in = io_cstr("/*\n"
		"command\n"
	       	"ident\n"
		"\n"
		"Some description\n"
		"$Copyright (C) Author\n"
		"Some description\n"
		"*/"
	);

	in = io_cmdpreproc(in);
	EshIdent(in);
	io_close(in);

	EXPECT_STR(CmdPar_getval(NULL, "Ident", NULL),
		"command\nident\n");
	EXPECT_STR(CmdPar_getval(NULL, "Copyright", NULL),
		"Copyright (C) Author");
}
