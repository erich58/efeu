#include <EFEU/UnitTest.h>
#include <unistd.h>

TEST_SUITE
{
	EXPECT(UnitTest_stdout("stdout"));
	EXPECT(UnitTest_stderr("stderr"));
	EXPECT(printf("Testnachricht\n") > 0);
	EXPECT(UnitTest_stdout(NULL)); // oder fflush(stdout) ! 
	EXPECT(fprintf(stderr, "Fehlermeldung\n") > 0);
	EXPECT_CONTENT("stdout", "Testnachricht\n");
	EXPECT_CONTENT("stderr", "Fehlermeldung\n");
	EXPECT(UnitTest_stdout("bad/file/name") == 0);
	EXPECT(fprintf(stderr, "Neuer Eintrag\n") > 0);
	EXPECT_CONTENT("stderr", "Fehlermeldung\nNeuer Eintrag\n");
	EXPECT(UnitTest_stderr("")); // == UnitTest_stderr("/dev/null")
	EXPECT(UnitTest_stderr(NULL)); // restore
}
