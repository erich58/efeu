#include <EFEU/UnitTest.h>
#include <unistd.h>

TEST_SUITE
{
	EXPECT(UnitTest_fderr() == 2);
	EXPECT(UnitTest_stdout("stdout"));
	EXPECT(UnitTest_stderr("stderr"));
	EXPECT(UnitTest_fderr() != 2);
	EXPECT(printf("Testnachricht\n") > 0);
	EXPECT(UnitTest_stdout(NULL)); // oder fflush(stdout) ! 
	EXPECT(fprintf(stderr, "Fehlermeldung\n") > 0);
	EXPECT_CONTENT("stdout", "Testnachricht\n");
	EXPECT_CONTENT("stderr", "Fehlermeldung\n");
	EXPECT(UnitTest_stdout("bad/file/name") == 0);
	EXPECT(UnitTest_stdout(".bad/file/name") == 0);
	EXPECT(fprintf(stderr, "Neuer Eintrag\n") > 0);
	EXPECT_CONTENT("stderr", "Fehlermeldung\nNeuer Eintrag\n");
	EXPECT(UnitTest_stderr("")); // "" =^= "/dev/null"
	EXPECT(UnitTest_stderr(NULL)); // restore
	EXPECT(UnitTest_stdout(".") == 0);
	EXPECT(UnitTest_stdout("..") == 0);

	EXPECT(UnitTest_stdin("", "unused")); // "" =^= "/dev/null"
	EXPECT(getchar() == EOF);

	char buf[64];
	TEST_FILE("input", "test data\n");
	EXPECT(UnitTest_stdin("input", NULL));
	EXPECT_STR(fgets(buf, sizeof buf, stdin), "test data\n");
	EXPECT(UnitTest_stdin("input", "other content\n"));
	EXPECT_STR(fgets(buf, sizeof buf, stdin), "other content\n");
	EXPECT(UnitTest_stdin(NULL, "unused"));	// restore
	TEST_FILE("input", NULL);
	EXPECT(UnitTest_stdin("input", NULL) == 0);
}
