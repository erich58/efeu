#include <EFEU/UnitTest.h>
#include <stdlib.h>

TEST_DECL(special)
{
	MATCH_LINES(NULL, NULL);

	TEST_FORK
	{
		MATCH_LINES("abc\nxyz\n", NULL);
		MATCH_LINES(NULL, "abc\nxyz\n");
		MATCH_LINES("", "abc\n[x\n.\n");
		TEST_EXIT;
	}

	CHILD_EXIT(1);
}

TEST_DECL(lmatch)
{
	const char *str = 
		"00.010300 DBG onAlarm: Timer 'OneShotTimer' triggered.\n" 
		"00.010320 LOG callbackTimer: OneShotTimer: 1\n"
		"00.010330 DBG onAlarm: Timer 'OneShotTimer' expired.\n" 
		"00.010400 DBG waitTime: 1 reduced to 1\n"
		"00.010500 DBG wait: No active listener\n";
	MATCH_LINES(str, 
		"DBG onAlarm: Timer 'OneShotTimer' triggered.\n" 
		"LOG callbackTimer: OneShotTimer: 1\n"
		"DBG onAlarm: Timer 'OneShotTimer' expired.\n" 
		"DBG waitTime: \n"
		"DBG wait: No active listener.\n"
	);
}

TEST_SUITE
{
	TEST_RUN(special);
	TEST_RUN(lmatch);
}
