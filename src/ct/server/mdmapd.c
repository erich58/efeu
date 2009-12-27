#include <EFEU/pconfig.h>
#include <EFEU/Resource.h>
#include <EFEU/Debug.h>
#include <EFEU/printobj.h>
#include <EFEU/mdmat.h>
#include <EFEU/StatData.h>
#include <EFEU/efutil.h>
#include <Math/TimeSeries.h>
#include <Math/mdmath.h>
#include <Math/pnom.h>
#include <Math/func.h>
#include "CubeHandle.h"

#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <unistd.h>

static LogControl err_ctrl = LOG_CONTROL("cube", LOGLEVEL_ERR);
static LogControl note_ctrl = LOG_CONTROL("cube", LOGLEVEL_NOTE);
static LogControl trace_ctrl = LOG_CONTROL("cube", LOGLEVEL_TRACE);

extern void process (int fd);

static int server = 1;

static void sig_int (int sig)
{
	char *p = GetResource("Config", NULL);
	IO *io = io_fopen(p, "r");
	MapTab_clean();
	log_note(&note_ctrl, "$!: reload $1\n", "s", p);
	MapTab_load(io);
	io_close(io);
}

static void remove_pid_file(void *pid_file)
{
	if	(server && pid_file)
	{
		remove(pid_file);
		log_note(&note_ctrl, "$!: pid file removed.\n", "s", pid_file);
	}
}


int main (int argc, char **argv)
{
	struct sockaddr_in address;
	socklen_t address_len = sizeof address;
	struct sigaction sigact, oldact;
	int sockfd; 
	int fd;
	int y;
	int pid;
	int single_session;
	IO *io;
	char *p;

	/*
	EfeuConfig("standalone");
	*/
	SetVersion("$Id: mdmapd.c,v 1.7 2009-09-27 05:46:13 ef Exp $");
	SetupStd();
	SetupUtil();

	SetupEDB();
	SetupTimeSeries();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();
	SetupMathFunc();
	SetupStatData();
	SetupDebug();
	ParseCommand(&argc, argv);

	if	(GetFlagResource("ListCommands"))
	{
		list_commands(iostd);
		return EXIT_SUCCESS;
	}

	if	((p = GetResource("PidFile", NULL)))
	{
		io = io_fileopen(p, "w");
		io_xprintf(io, "%u\n", (unsigned) getpid());
		io_close(io);
		proc_clean(remove_pid_file, mstrcpy(p));
	}

	io = io_fopen(GetResource("Config", NULL), "r");
	MapTab_load(io);
	io_close(io);

	/* create socket connection */	

	if 	((sockfd = socket (PF_INET, SOCK_STREAM, 0)) < 0) 
	{
		log_error(&err_ctrl, "$!: socket: $1\n", "s", strerror(errno));
		exit (EXIT_FAILURE);
	}

	log_note(&trace_ctrl, "$!: socket created successfully.\n", NULL);

	address.sin_family = AF_INET;
	address.sin_port = htons (GetIntResource("Port", 2000));
	memset (&address.sin_addr, 0, sizeof address.sin_addr);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof y);

	if	(bind (sockfd, (struct sockaddr *) &address, sizeof address)) 
	{
		log_error(&err_ctrl, "$!: bind: $1\n", "s", strerror(errno));
		exit (EXIT_FAILURE);
	}

	log_note(&trace_ctrl, "$!: socket server is ready.\n", NULL);

	if 	(listen (sockfd, 5)) 
	{
		log_error(&err_ctrl, "$!: listen: $1\n", "s", strerror(errno));
		exit (EXIT_FAILURE);
	}

	/* avoid zombies (note: may not work on all unix systems) */

	signal(SIGCHLD, SIG_IGN);
	sigact.sa_handler = sig_int;
	sigaction(SIGQUIT, &sigact, &oldact);

	/* wait for connections to come in */

	single_session = GetFlagResource("SingleSession");

	for (;;)
	{
		fd = accept (sockfd, (struct sockaddr *) &address,
			&address_len);

		/* sanity check to be sure that nothing goes wrong */

		if 	(fd < 0) 
		{
			switch (errno)
			{
			case EINTR:
				fprintf(stderr, "server: interrupt\n");
				continue;
			default:
				log_error(&err_ctrl, "$!: accept: $1\n",
					"s", strerror(errno));
				exit (EXIT_FAILURE);
			}
		}

		pid = single_session ? 0 : fork();

		if	(pid == 0)
		{
			server = 0;
			sigaction(SIGHUP, &sigact, &oldact);
			sigaction(SIGINT, &sigact, &oldact);
			process(fd);
			close(fd);
			exit(EXIT_SUCCESS);
		}

		log_note(&note_ctrl, "$!: connect pid=$1\n", "d", pid);
		close(fd);
	}
	
	return EXIT_SUCCESS;
}
