#include <EFEU/UnitTest.h>
#include "src2doc.h"

TEST_SUITE
{
	TEST_FILE("input",
		"#!/bin/sh\n"
		"# Hello World\n"
		"# $Copyright (c) Name\n"
		"\n"
		"# $Description\n"
		"# Hello World Example\n"
		"echo Hello World\n"
	);

//	InsertCode = 1;
	IO *in = io_fileopen(UnitTest_tmp("input"), "r");
	IO *out = io_fileopen(UnitTest_tmp("output"), "w");
	s2d_sh("hello.sh", in, out);
	io_close(in);
	io_close(out);
	EXPECT_CONTENT("output",
		"\\mpage[7] hello.sh\n"
		"\\Name\n"
		"hello.sh -- Hello World\n"
		"\n"
		"\\Description\n"
		"\n"
		"Hello World Example\n"
		"\\Copyright\n"
		"Copyright (c) Name\n"
	);
}
