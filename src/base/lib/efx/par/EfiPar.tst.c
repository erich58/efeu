#include <EFEU/UnitTest.h>
#include <EFEU/EfiPar.h>
#include <EFEU/utest/out.tst.h>

EfiPar EfiPar_t1 = { "t1", "Test 1" };
EfiPar EfiPar_t2 = { "t2", "Test 2" };

typedef struct {
	EPC_VAR;
	char *desc;
} TestPar;

static void TestPar_info (IO *out, const void *data)
{
	const TestPar *tpar = data;
	io_puts(tpar->epc_name, out);
	io_puts("[", out);
	io_puts(tpar->epc_label, out);
	io_puts("] ", out);
	io_puts(tpar->desc, out);
	io_puts("\n", out);
}

static void visit(EfiParClass *class, void *data)
{
	TestPar_info(data, class);
}

TestPar TestPar_a = {
	EPC_DATA(&EfiPar_t1, NULL, "a", "A", TestPar_info), "A-Desc",
};

TestPar TestPar_tab[] = {
	{ EPC_DATA(&EfiPar_t1, NULL, "b", "B", TestPar_info), "B-Desc" },
	{ EPC_DATA(&EfiPar_t2, NULL, "c", "C", NULL), "C-Desc" },
	{ EPC_DATA(&EfiPar_t2, NULL, "d", "D", NULL), "D-Desc" },
	{ EPC_DATA(&EfiPar_t2, NULL, "e", "E", NULL), "E-Desc" },
};

TEST_SUITE
{
	AddEfiPar(&Type_int, &TestPar_a);

	ListEfiPar(OUT(), &Type_int, NULL, "pfx", 0);
	EXPECT_OUT("\nTest 1\n\npfx.a\n");
	ListEfiPar(OUT(), &Type_int, NULL, "pfx", 1);
	EXPECT_OUT("\nTest 1\n\npfx.a\tA\n");
	ListEfiPar(OUT(), &Type_int, NULL, "pfx", 2);
	EXPECT_OUT("\nTest 1\n\na[A] A-Desc\n");

	EXPECT(SearchEfiPar(&Type_int, &EfiPar_t1, NULL) == NULL);
	UnitTest_stderr("stderr");

	TEST_FORK
	{
		EXPECT(SearchEfiPar(&Type_int, &EfiPar_t1, "?") == NULL);
		exit(1); /* Not reached */
	}

	CHILD_EXIT(0);
	UnitTest_stderr(NULL);
	EXPECT_CONTENT("stderr", "a\n");
	EXPECT(SearchEfiPar(&Type_int, &EfiPar_t1, "a") != NULL);
	EXPECT(SearchEfiPar(&Type_int, &EfiPar_t2, "a") == NULL);

	AddEfiParTab(NULL, TestPar_tab, 4, sizeof TestPar_tab[0]);
	ListEfiPar(OUT(), NULL, NULL, NULL, 0);
	EXPECT_OUT("\nTest 1\n\n"
		"b\n"
		"\nTest 2\n\n"
		"c            d            e\n"
	);
	ListEfiPar(OUT(), NULL, &EfiPar_t1, NULL, 2);
	EXPECT_OUT("b[B] B-Desc\n");
	ListEfiPar(OUT(), NULL, &EfiPar_t2, "pfx", 0);
	EXPECT_OUT(
"pfx.c        pfx.d        pfx.e\n"
);
	ListEfiPar(OUT(), NULL, &EfiPar_t2, "123456789012", 0);
	EXPECT_OUT(
"123456789012.c            123456789012.d            123456789012.e\n"
);
	ListEfiPar(OUT(), NULL, &EfiPar_t2, "1234567890123456789012345", 0);
	EXPECT_OUT(
"1234567890123456789012345.c            1234567890123456789012345.d\n"
"1234567890123456789012345.e\n"
);

	PrintEfiPar(OUT(), TestPar_tab);
	EXPECT_OUT("b[B] B-Desc\n");
	PrintEfiPar(OUT(), TestPar_tab + 1);
	EXPECT_OUT("c\n\tC\n");

	EfiParWalk(&Type_int, NULL, visit, OUT());
	EXPECT_OUT("a[A] A-Desc\n");

	EfiParWalk(NULL, NULL, visit, OUT());
	EXPECT_OUT("b[B] B-Desc\n"
		"c[C] C-Desc\n"
		"d[D] D-Desc\n"
		"e[E] E-Desc\n"
	);

	EXPECT(GetEfiPar(NULL, &EfiPar_t2, "b") == NULL);
	EXPECT(GetEfiPar(NULL, &EfiPar_t2, "c") != NULL);
	
	CLEANUP_OUT;
}
