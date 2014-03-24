/*
Log-Datei öffnen

$Copyright (C) 2009 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/ftools.h>
#include <EFEU/io.h>
#include <sys/types.h>
#include <sys/wait.h>

#define	FMT_ERR	"[ftools:5]$!: could not create temporary file.\n"


FILE *log_file (LogControl *ctrl)
{
	FILE *file;
	IO *out;
	char *p;
	int fd[2];
	pid_t pid;
	int last, c;

	out = LogOpen(ctrl);

	if	(!out)	return NULL;

	pipe(fd);
	pid = fork();

/*	Parent
*/
	if	(pid > 0)
	{
		int status;

		io_close(out);
		close(fd[0]);
		file = fdopen(fd[1], "w");
		setvbuf(file, NULL, _IOLBF, 0);
		p = rd_ident(ctrl);
		filenotice(p, "w+b", file, fclose);
		memfree(p);
		waitpid(pid, &status, 0);
		return file;
	}
	else if	(pid < 0)
	{
		return NULL;
	}

/*	Doppelfork zur Vermeidung von Zombies
*/
	if	(fork() != 0)
		exit(EXIT_FAILURE);

	close(0);
	dup2(fd[0], 0);
	close(fd[0]);
	close(fd[1]);

	for (last = 0; (c = getchar()) != EOF; last = c)
	{
		if	(last == '\n')
		{
			io_close(out);
			out = LogOpen(ctrl);
		}

		io_putc(c, out);
	}

	io_close(out);
	exit(EXIT_SUCCESS);
	return NULL;
}
