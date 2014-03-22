/*
:*:	show/change time stamp of compressd file
:de:	Abfragen/Ändern des Zeitstempels komprimierter Dateien

$Copyright (C) 2004 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#define _XOPEN_SOURCE 1

#include <EFEU/Resource.h>
#include <EFEU/procenv.h>
#include <EFEU/ftools.h>
#include <EFEU/strbuf.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utime.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define	HSIZE	10

#define	SET	"[zstamp:set]$1: time stamp modified.\n"
#define	ISDIR	"[zstamp:isdir]$1: directory skipped.\n"
#define	MAGIC	"[zstamp:magic]$1: file not compressed with gzip.\n"
#define	WERR	"[zstamp:werr]$1: output error, file may be corrupted!\n"

static void show_stamp (const char *pfx, time_t tval)
{
	struct tm *tp;

	tp = localtime(&tval);
	printf("%s%02d.%02d.%02d %02d:%02d:%02d", pfx,
		tp->tm_mday, 1 + tp->tm_mon, tp->tm_year % 100,
		tp->tm_hour, tp->tm_min, tp->tm_sec);
}


int main (int argc, char **argv)
{
	int i;
	char *src;
	unsigned char buf[HSIZE];
	struct stat sbuf;
	struct utimbuf ubuf;
	time_t stamp;
	FILE *file;
	int sync, clear, list, allfiles, notbest, nostamp, touch;
	int set_utime;
	int k;

	SetProgName(argv[0]);
	SetVersion(EFEU_VERSION);
	ParseCommand(&argc, argv);
	sync = GetFlagResource("SyncStamp");
	clear = GetFlagResource("ClearStamp");
	list = GetFlagResource("ListStamp");
	allfiles = GetFlagResource("AllFiles");
	notbest = GetFlagResource("NotBest");
	nostamp = GetFlagResource("NoStamp");
	touch = GetFlagResource("Touch");

	for (i = 1; i < argc; i++)
	{
		src = argv[i];

		if	(stat(src, &sbuf) != 0)
		{
			perror(src);
			continue;
		}

		if	(S_ISDIR(sbuf.st_mode))
		{
			log_note(NULL, ISDIR, "s", src);
			continue;
		}

		file = fopen(src, sync || clear ? "rb+" : "rb");

		if	(!file)
		{
			perror(src);
			continue;
		}

		k = fread(buf, 1, HSIZE, file);

		if	(k != HSIZE || buf[0] != 037 || buf[1] != 0213)
		{
			fclose(file);
			log_note(NULL, MAGIC, "s", src);
			continue;
		}

		stamp  = buf[4];
		stamp += buf[5] << 8;
		stamp += buf[6] << 16;
		stamp += buf[7] << 24;
		set_utime = 0;

		if	(clear)
		{
			stamp = 0;
			buf[4] = 0;
			buf[5] = 0;
			buf[6] = 0;
			buf[7] = 0;
		}
		else if	(sync)
		{
			stamp = sbuf.st_mtime;
			buf[4] = stamp & 0xFF;
			buf[5] = (stamp >> 8) & 0xFF;
			buf[6] = (stamp >> 16) & 0xFF;
			buf[7] = (stamp >> 24) & 0xFF;
		}
		else if	(touch && stamp)
		{
			sbuf.st_mtime = stamp;
			set_utime = 1;
		}
		else if	(allfiles || (!stamp == nostamp) || buf[3] & 0x08 ||
			(notbest && buf[8] != 2))
		{
			printf("%s:", src);

			switch (buf[8])
			{
			case 0: 
				fputs(" [medium compression]", stdout);
				break;
			case 2: 
				fputs(" [max compression]", stdout);
				break;
			case 4: 
				fputs(" [max speed]", stdout);
				break;
			default:
				printf(" [%x]", buf[8]);
				break;
			}

			if	(stamp)
				show_stamp(" ", stamp);

			if	(buf[3] & 0x08)
			{
				int c;

				putchar(' ');

				while ((c = getc(file)) != EOF && c != 0)
					putchar(c);
			}

			putchar('\n');
		}

		if	(clear || sync)
		{
			rewind(file);
			k = fwrite(buf, 1, HSIZE, file);
			set_utime = 1;

			if	(k != HSIZE)
			{
				fclose(file);
				log_error(NULL, WERR, "s", src);
			}
			else	log_note(NULL, SET, "s", src);
		}

		if	(set_utime)
		{
			ubuf.actime = sbuf.st_atime;
			ubuf.modtime = sbuf.st_mtime;

			if	(utime(src,  &ubuf) != 0)
				perror(src);
		}

		fclose(file);

		if	(list)
		{
			printf("%s:", src);
			show_stamp(" mtime=", sbuf.st_mtime);
			show_stamp(" stamp=", stamp);
			putchar('\n');
		}
	}

	return EXIT_SUCCESS;
}
