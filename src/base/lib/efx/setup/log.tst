#!/usr/bin/env .build/script.tst

UnitTest::stderr("err.out");
LogOutputStat();
UnitTest::stderr();
MATCH_CONTENT("err.out", "stderr 0");

EXPECT_REPR(ErrLog, "LogControl: .err");
EXPECT_REPR(WarnLog, "LogControl: .warn");
EXPECT_REPR(NoteLog, "LogControl: .note");
EXPECT_REPR(InfoLog, "LogControl: .info");
EXPECT_REPR(DebugLog, "LogControl: .debug");
EXPECT_REPR(TraceLog, "LogControl: .trace");

TEST_FILE("log.out", NULL);
LogConfig(UnitTest::tpl(".note=@/log.out"));
EXPECT_STR(NoteLog, "(null).note");
fprintf(NoteLog, "Logeintrag");
EXPECT_CONTENT("log.out", "Logeintrag\n");

TEST_FILE("log.out", NULL);
IO io = NoteLog.open();
fprintf(io, "result=%d\n", 42);
close(io);
sleep(0.1);
EXPECT_CONTENT("log.out", "result=42\n");

LogControl test;
EXPECT_STR(test, "NULL");
test = LogControl("test.note");
EXPECT_STR(test, "test.note");
