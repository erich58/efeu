#include <UnitTest.h>

TEST_DECL(data)
{
	TEST_IO_DECL(io);

	EXPECT_STR(UnitTest_data(&io), NULL);

	EXPECT_INT(UnitTest_init(&io, "Test content\n", 0), 13);
	EXPECT_STR(UnitTest_data(&io), "Test content\n");
	EXPECT_INT(UnitTest_append(&io, "End\nExtra", 4), 4);
	EXPECT_STR(UnitTest_data(&io), "Test content\nEnd\n");

	EXPECT_INT(UnitTest_init(&io, "New data\n", 0), 9);
	EXPECT_STR(UnitTest_data(&io), "New data\n");

	EXPECT_INT(UnitTest_init(&io, NULL, 0), EOF);
	EXPECT_STR(UnitTest_data(&io), NULL);
}

TEST_DECL(read)
{
	char buf[1024] = {};
	TEST_IO_DECL(io);

	EXPECT_INT(UnitTest_init(&io,
		"First line\nSecond line\nLast line\n", 0), 33);

	FILE *file = UnitTest_fopen(&io, "r");
	EXPECT_STR(fgets(buf, sizeof buf, file), "First line\n");
	EXPECT_STR(fgets(buf, sizeof buf, file), "Second line\n");
	EXPECT_STR(fgets(buf, sizeof buf, file), "Last line\n");
	EXPECT_STR(fgets(buf, sizeof buf, file), NULL);
	fclose(file);

	UnitTest_rewind(&io);
	memset(buf, 0, sizeof buf);
	EXPECT_INT(read(UnitTest_fd(&io), buf, sizeof buf), 33);
	EXPECT_STR(buf, "First line\nSecond line\nLast line\n");

	EXPECT_INT(UnitTest_init(&io, "New data\n", 0), 9);
	int fd = UnitTest_open(&io);
	memset(buf, 0, sizeof buf);
	EXPECT_INT(read(fd, buf, sizeof buf), 9);
	close(fd);
	EXPECT_STR(buf, "New data\n");
	UnitTest_init(&io, NULL, 0);
}

TEST_DECL(write)
{
	TEST_IO_DECL(io);

	fputs("first line\n", UnitTest_file(&io));
	fputs("second line\n", UnitTest_file(&io));
	EXPECT_STR(UnitTest_data(&io), "first line\nsecond line\n");

	FILE *file = UnitTest_fopen(&io, "w");
	fputs("some data\n", file);
	fclose(file);
	EXPECT_STR(UnitTest_data(&io), "some data\n");

	file = UnitTest_fopen(&io, "a");
	fputs("appendix\n", file);
	fclose(file);
	EXPECT_STR(UnitTest_data(&io), "some data\nappendix\n");

	UnitTest_init(&io, NULL, 0);
	EXPECT_INT(write(UnitTest_fd(&io), "data", 4), 4);
	EXPECT_STR(UnitTest_data(&io), "data");
	EXPECT_INT(write(UnitTest_fd(&io), "\nextra\n", 7), 7);
	EXPECT_STR(UnitTest_data(&io), "data\nextra\n");

	UnitTest_init(&io, NULL, 0);
	int fd = UnitTest_open(&io);
	EXPECT_INT(write(fd, "something\n", 10), 10);
	close(fd);
	EXPECT_STR(UnitTest_data(&io), "something\n");
	UnitTest_init(&io, NULL, 0);
}

TEST_SUITE
{
	TEST_RUN(data);
	TEST_RUN(read);
	TEST_RUN(write);
}
