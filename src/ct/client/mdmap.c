#include <EFEU/pconfig.h>
#include <EFEU/Resource.h>
#include <EFEU/Debug.h>
#include <EFEU/strbuf.h>
#include <EFEU/parsearg.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <errno.h>
#include <fcntl.h>
#include <EFEU/stack.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

int usleep(useconds_t usec);

#define	BSIZE	32

static struct sockaddr_in address;
static IO *log_out = NULL;
static useconds_t usec = 0;

static int connect_flag = 0;
static int verbose_flag = 0;
static int single_flag = 0;

static void show_data (const char *buf)
{
	int i;
	int at_start;

	if	(buf)
	{
		at_start = 1;

		for (i = 0; buf[i]; i++)
		{
			if	(at_start)
				io_puts("< ", log_out);

			at_start = (buf[i] == '\n');
			io_putc(buf[i], log_out);
		}

		if	(!at_start)
			io_puts("\\\n", log_out);
	}
	else	io_puts("*EOF\n", log_out);
}


static int do_connect (void)
{
	int fd;

	if	(!connect_flag)	return EOF;

	if 	((fd = socket (PF_INET, SOCK_STREAM, 0)) < 0) 
		log_error(NULL, "$!: socket: $1\n", "s", strerror(errno));

	if	(connect(fd, (struct sockaddr *) &address, sizeof address))
		log_error(NULL, "$!: connect: $1\n", "s", strerror(errno));

	io_xprintf(log_out, "* connect\n");
	return fd;
}

static void close_connect (int fd)
{
	if	(fd != EOF)
	{
		io_xprintf(log_out, "* close\n");
		close(fd);
	}
}

static void send_data (int fd, const char *p)
{
	int fx;

	if	(!p)	return;

	fx = fd == EOF ? do_connect() : fd;

	io_xprintf(log_out, "> %s\n", p);

	if	(fx != EOF && write(fx, p, strlen(p) + 1) <= 0)
		log_error(NULL, "$!: write: $1\n", "s", strerror(errno));

	if	(usec)
		usleep(usec);

	if	(fx != EOF)
		show_data(sb_read(fx, NULL));

	if	(fd == EOF)
		close_connect(fx);
}

int main (int argc, char **argv)
{
	IO *in;
	char *p;
	int count;
	int fd;
	struct hostent *hp;

	/*
	EfeuConfig("standalone");
	*/
	SetVersion(EFEU_VERSION);
	ParseCommand(&argc, argv);

	SetupStd();
	SetupReadline();
	SetupDebug();

	address.sin_family = AF_INET;
	address.sin_port = htons (GetIntResource("Port", 2000));
	memset (&address.sin_addr, 0, sizeof address.sin_addr);

	if	((p = GetResource("Host", NULL)))
	{
		if	(!(hp = gethostbyname(p)))
			log_error(NULL, "$!: gethostbyname: $1 not found.\n",
				"s", p);

		memcpy(&address.sin_addr, hp->h_addr_list[0],
			sizeof address.sin_addr);
	}

	usec = 1000 * GetIntResource("SleepTime", 0);
	count = GetIntResource("Repeat", 1);
	connect_flag = GetFlagResource("Connect");
	verbose_flag = GetFlagResource("Verbose");
	single_flag = GetFlagResource("Single");

	p = GetResource("Ouput", NULL);
	log_out = p ? io_fileopen(p, "w") : rd_refer(iostd);

	p = GetResource("Command", NULL);
	fd = single_flag ? EOF : do_connect();

	if	(p)
	{
		do
		{
			send_data(fd, p);
			count--;
		}
		while (count > 0);
	}
	else
	{
		if	((p = GetResource("Input", NULL)))
		{
			in = io_fileopen(p, "r");
		}
		else
		{
			in = io_interact("cube: ", "~/.cube_history");
			send_data(fd, "set lines=20");
		}

		while ((p = io_getline(in, NULL, '\n')))
		{
			send_data(fd, p);
			memfree(p);
		}

		io_close(in);
	}

	close_connect(fd);
	io_close(log_out);
	return EXIT_SUCCESS;
}
