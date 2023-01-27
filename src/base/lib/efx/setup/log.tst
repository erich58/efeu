#!/usr/bin/env .build/script.tst

EXPECT_REPR(ErrLog, "LogControl: .ERR");
EXPECT_REPR(WarnLog, "LogControl: .WNG");
EXPECT_REPR(NoteLog, "LogControl: .MSG");
EXPECT_REPR(InfoLog, "LogControl: .INF");
EXPECT_REPR(DebugLog, "LogControl: .DBG");
EXPECT_REPR(TraceLog, "LogControl: .TRC");

TEST_FILE("log.out", NULL);
LogConfig(UnitTest::tpl(".note=@/log.out"));
EXPECT_STR(NoteLog, "MSG=0");
fprintf(NoteLog, "Logeintrag");
MATCH_CONTENT("log.out", "Logeintrag\n");

TEST_FILE("log.out", NULL);
IO io = NoteLog.open();
fprintf(io, "result=%d\n", 42);
close(io);
sleep(0.1);
MATCH_CONTENT("log.out", "result=42\n");

LogControl test;
EXPECT_STR(test, "NULL");
test = LogControl("test.note");
EXPECT_STR(test, "test.MSG=0");
