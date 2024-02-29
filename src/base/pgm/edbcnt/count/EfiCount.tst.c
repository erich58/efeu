#include <EFEU/UnitTest.h>
#include <EFEU/EfiCount.h>
#include <EFEU/Info.h>
#include "utest/out.tst.h"

EfiCount test_count = EFI_COUNT(NULL,
	"test_count", "[opt]=arg", NULL, "some parameter",
	"Test Counter",
	"id=$1\npar=$2\nundefined=$x."
);

TEST_SUITE
{
	SetupEfiCount();
	AddEfiPar(NULL, &test_count);
	DumpInfo(OUT(), NULL, "count", 0);
	EXPECT_OUT(
		"@: data count method\n"
		"@record| --info=dump:/count/record\n"
		"@test_count| --info=dump:/count/test_count\n"
	);

	PrintInfo(OUT(), rd_deref(GetInfo(NULL, "count/record")));
	EXPECT_OUT(
		"count records\n"
		"record\n"
		"\tcount records\n"
	);

	PrintInfo(OUT(), rd_deref(GetInfo(NULL, "count/test_count")));
	EXPECT_OUT(
		"Test Counter\n"
		"test_count[opt]=arg\n"
		"\tid=test_count\n"
		"\tpar=some parameter\n"
		"\tundefined=x.\n"
	);

	CLEANUP_OUT;
}
