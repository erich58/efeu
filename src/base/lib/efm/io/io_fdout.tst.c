#define _GNU_SOURCE

#include <EFEU/UnitTest.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

IO *io_fdout(int fd, size_t blksize);

static unsigned pending(IO *io)
{
	return io_ctrl(io, IO_PENDING);
}

TEST_DECL(file)
{
	FILE *out = fopen(UnitTest_tmp("data"), "w");
	IO *io = io_fdout(fileno(out), 8);

	EXPECT(io_ctrl(io, IO_FDOUT) == fileno(out));
	MATCH_STR(io_xident(io, NULL), "fdout\\([0-9]*)");
	EXPECT(io_ctrl(io, IO_REWIND) == -1);

	io_puts("abc", io);
	EXPECT_INT(pending(io), 3);
	EXPECT_INT(io_flush(io), 0);
	EXPECT_INT(pending(io), 0);
	io_puts("xyz\n", io);
	EXPECT_INT(pending(io), 0);
	io_puts("1234567890", io);
	EXPECT_INT(pending(io), 2);
	io_putc('\n', io);
	EXPECT_INT(pending(io), 0);
	io_close(io);

	fclose(out);
	PRINT_CONTENT("data");
}

TEST_DECL(pipe)
{
	int fd[2];
	ASSERT(pipe2(fd, O_NONBLOCK) == 0);

	int psize = getpagesize();
	PRINT_INT(psize);
	PRINT_INT(fcntl(fd[0], F_SETPIPE_SZ, psize));
	EXPECT_INT(fcntl(fd[0], F_GETPIPE_SZ), psize);
	
	IO *io = io_fdout(fd[1], 8);

	io_puts("1234567\n", io);
	EXPECT_INT(io_flush(io), 0);
	io_puts("1234567\n", io);
	EXPECT_INT(io_flush(io), EOF);
	EXPECT(errno == EAGAIN);
	EXPECT_INT(pending(io), 8);

	int n = 0;

	while (n < psize)
		n += io_puts("1234567\n", io);

	EXPECT_INT(io_flush(io), EOF);
	EXPECT(errno == EAGAIN);
	EXPECT_INT(pending(io), psize + 8);

	char *data = malloc(2 * psize);
	EXPECT_INT(read(fd[0], data, 2 * psize), 8);
	EXPECT_INT(io_flush(io), EOF);
	EXPECT(errno == EAGAIN);
	EXPECT_INT(read(fd[0], data, 2 * psize), psize);
	EXPECT_INT(io_flush(io), 0);
	EXPECT_INT(read(fd[0], data, 2 * psize), 8);

	io_close(io);
	close(fd[0]);
}

TEST_DECL(socket)
{
	char data[4096];

	int socket[2];
	ASSERT(socketpair(AF_UNIX, SOCK_STREAM|SOCK_NONBLOCK, 0, socket) == 0);

	IO *io = io_fdout(socket[1], 0);

	int n = 0;
	int k = 0;

	while (io_flush(io) == 0)
		n += io_puts("1234567\n", io);

	PRINT_INT(n);
	k = pending(io);
	PRINT_INT(k);

	EXPECT_INT(read(socket[0], data, sizeof data), n - k);
	EXPECT_INT(io_ctrl(io, IO_FLUSH), 0);
	EXPECT_INT(read(socket[0], data, sizeof data), k);
	EXPECT_INT(read(socket[0], data, sizeof data), -1);
	EXPECT_INT(errno, EAGAIN);

	close(socket[0]);
	close(socket[1]);
}

TEST_SUITE
{
	TEST_RUN(file);
	TEST_RUN(pipe);
	TEST_RUN(socket);
}
