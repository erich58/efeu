#include <EFEU/UnitTest.h>
#include <EFEU/EfiPar.h>
#include <EFEU/utest/out.tst.h>

static EfiControl EfiControl_a = EPC_CONTROL(NULL, "a", "A-Desc", "A-Data");

TEST_SUITE
{
	AddEfiPar(NULL, &EfiControl_a);
	EfiControl_info(OUT(), &EfiControl_a);
	EXPECT_OUT(
		"a=\"A-Data\"\n"
		"\tA-Desc\n"
	);

	EXPECT(GetEfiControl(&Type_int, "a") == NULL);
	EXPECT(GetEfiControl(NULL, "a") != NULL);
	EXPECT(SearchEfiControl(&Type_int, "a") != NULL);
	EXPECT(SearchEfiControl(NULL, "a") != NULL);
	CLEANUP_OUT;
}
