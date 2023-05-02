/* UnitTest für CmdParExpand
*/

#include <EFEU/UnitTest.h>
#include <EFEU/CmdPar.h>
#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/procenv.h>
#include "utest/out.tst.h"

static CmdPar par;

static void pset(const char *name, const char *value)
{
	CmdPar_setval(&par, name, mstrcpy(value));
}

static void parg(const char *str)
{
	IO *io = io_cstr(str);
	CmdPar_read(&par, io, EOF, 1);
	io_close(io);
}

static const char *pexpand(const char *str)
{
	IO *io = io_cstr(str);
	CmdPar_expand(&par, io, OUT());
	io_close(io);
	return utest_out();
}

static const char *expand(const char *name, const char *arg)
{
	CmdParExpand *def = CmdParExpand_get(name);

	if	(def && def->eval)
	{
		def->eval(&par, OUT(), arg);
		return utest_out();
	}

	return NULL;
}

static void f_test_add (CmdPar *par, IO *io, const char *arg)
{
	io_puts(arg, io);
}

static CmdParExpand test_add = { "test_add", "Testfunktion", f_test_add };

TEST_SUITE
{
	CmdParExpand_show(OUT(), NULL);	// Für Testabdeckung //
	MATCH_OUT("^\\[arglist] .*\n\\[version]");
	CmdParExpand_show(OUT(), "$1 ");
	EXPECT_OUT("arglist copyright environ head ident name "
		"options synopsis varlist version ");

	CmdPar_init(&par);

	pset(NULL, "CommandName");
	parg("o|OPT\n\tOption\n");
	parg(":arg|ARG\n\tArgument\n");
	parg("@env|ENV\n\tEnvironment\n");

	pset("Ident", "IdentName");
	pset("Version", "UnitTest 1.0");
	pset("Copyright", "Copyright (C) Erich Frühstück");

	pset(".manpage.head", "mpage[1] $1\n");
	pset(".manpage.name", "NAME\n");
	pset(".synopsis.head", "SYNOPSIS\n");
	pset(".synopsis.intro", "usage:\n");
	pset(".arglist.head", "DESCRIPTION\n");
	pset(".arglist.intro", "description:\n");
	pset(".environ.head", "ENVIRONMENT\n");
	pset(".version.head", "VERSION\n");
	pset(".copyright.head", "COPYRIGHT\n");

	CmdPar_docmode = 0;
	EXPECT_STR(expand("head", "unused"), "mpage[1] CommandName\n");
	EXPECT_STR(expand("ident", "unused"), "CommandName -- IdentName\n");
	EXPECT_STR(expand("name", "unused"),
		"mpage[1] CommandName\nNAME\nCommandName -- IdentName\n");

	EXPECT_STR(expand("synopsis", "-h"), "SYNOPSIS\n"
		"CommandName [ -o ] arg\n");
	EXPECT_STR(expand("synopsis", "-i"), "usage:\n"
		"CommandName [ -o ] arg\n");
	EXPECT_STR(expand("synopsis", "-hi"), "SYNOPSIS\nusage:\n"
		"CommandName [ -o ] arg\n");
	EXPECT_STR(expand("synopsis", NULL),
		"CommandName [ -o ] arg\n");

	EXPECT_STR(expand("arglist", "-h"), "DESCRIPTION\n"
		"-o\n\tOption\narg\n\tArgument\n");
	EXPECT_STR(expand("arglist", "-i"), "description:\n"
		"-o\n\tOption\narg\n\tArgument\n");
	EXPECT_STR(expand("arglist", "-hi"), "DESCRIPTION\ndescription:\n"
		"-o\n\tOption\narg\n\tArgument\n");
	EXPECT_STR(expand("arglist", NULL),
		"-o\n\tOption\narg\n\tArgument\n");

	EXPECT_STR(expand("options", "unused"), "-o\n\tOption\n");

	EXPECT_STR(expand("environ", "-h"), "ENVIRONMENT\n"
		"env\n\tEnvironment\n");
	EXPECT_STR(expand("environ", NULL),
		"env\n\tEnvironment\n");

	EXPECT_STR(expand("version", "-h"), "VERSION\n"
		"UnitTest 1.0\n");
	EXPECT_STR(expand("version", NULL),
		"UnitTest 1.0\n");

	EXPECT_STR(expand("copyright", "-h"), "COPYRIGHT\n"
		"Copyright (C) Erich Frühstück\n");
	EXPECT_STR(expand("copyright", NULL),
		"Copyright (C) Erich Frühstück\n");

	EXPECT_STR(expand("varlist", "$1=$2;"),
		".arglist.head=DESCRIPTION\n;"
		".arglist.intro=description:\n;"
		".copyright.head=COPYRIGHT\n;"
		".environ.head=ENVIRONMENT\n;"
		".manpage.head=mpage[1] $1\n;"
		".manpage.name=NAME\n;"
		".synopsis.head=SYNOPSIS\n;"
		".synopsis.intro=usage:\n;"
		".version.head=VERSION\n;"
		"ARG=;"
		"Copyright=Copyright (C) Erich Frühstück;"
		"ENV=;"
		"Ident=IdentName;"
		"OPT=;"
		"Version=UnitTest 1.0;"
		);

	EXPECT_STR(expand("test_add", "DATA"), NULL);
	CmdParExpand_add(&test_add);
	EXPECT_STR(expand("test_add", "DATA"), "DATA");

	CmdPar_expand(NULL, NULL, OUT());
	EXPECT_OUT("@");
	EXPECT_STR(pexpand(""), "@");
	EXPECT_STR(pexpand(" "), "@");
	EXPECT_STR(pexpand("@"), "@");
	EXPECT_STR(pexpand("ident"), "CommandName -- IdentName\n");
	EXPECT_STR(pexpand("!"), "|CommandName|");
	EXPECT_STR(pexpand("{Ident}"), "IdentName");
	EXPECT_STR(pexpand("{Ident"), "IdentName");

	IO *io = io_cstr("@ident");
	CmdPar_iousage(&par, OUT(), io);
	io_close(io);
	EXPECT_OUT("CommandName -- IdentName\n");

	TEST_FILE("CommandName.hlp", "HELP\n");

	TEST_FORK
	{
		SetApplPath(UnitTest_tmp(NULL));
		EXPECT_STR(CmdPar_hpath(&par),
			UnitTest_tpl("@/CommandName.hlp"));
		CmdPar_manpage(&par, OUT());
		EXPECT_OUT("HELP\n");
		TEST_EXIT;
	}

	CHILD_EXIT(0);

	CmdPar_clean(&par);
	CLEANUP_OUT;
}
