/*
:*:log time stamp
:de:Zeitstempel für Logausgaben

$Copyright (C) 2023 Erich Frühstück
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

#define	_ISOC99_SOURCE

#include <EFEU/extension.h>
#include <EFEU/LogConfig.h>
#include <time.h>

#if	HAS_TIMEVAL
#include <sys/time.h>
#else
struct timeval {
	time_t tv_sec;
	unsigned long tv_usec;
};
#endif

/* Die Funktion generiert einen Zeitstempel der Form
 *  JJJJ-MM-DD hh:mm:ss.ssssss.
 *
 * Ist syncTime nicht NULL und (Format JJJJMMDDhhmmss) wird der
 * Zeitstempel relativ zu dieser Zeit errechnet.
 */

const char *log_timestamp(const char *syncTime)
{
	static char buf[32];
	static long offset_sec = 0;
	static long offset_usec = 0;
	struct timeval tv;

#if	HAS_GETTIMEOFDAY
	gettimeofday(&tv, NULL);
#else
	tv.tv_sec = time(NULL);
	tv.tv_usec = 0;
#endif

	if	(syncTime)
	{
		unsigned long long int stamp = strtoull(syncTime, NULL, 10);

		if	(stamp >= 19700101000000ull)
		{
			struct tm t = {};
			t.tm_sec = stamp % 100ull;
			stamp /= 100ull;
			t.tm_min = stamp % 100ull;
			stamp /= 100ull;
			t.tm_hour = stamp % 100ull;
			stamp /= 100ull;
			t.tm_mday = stamp % 100ull;
			stamp /= 100ull;
			t.tm_mon = stamp % 100ull - 1;
			stamp /= 100ull;
			t.tm_year = stamp - 1900;
			t.tm_isdst = -1;

			offset_sec = mktime(&t) - tv.tv_sec - 1;
			offset_usec = 1000000 - tv.tv_usec;
		}
		else
		{
			offset_sec = 0;
			offset_usec = 0;
		}
	}

	if	(offset_usec)
	{
		long usec = tv.tv_usec + offset_usec;
		tv.tv_usec = usec % 1000000;
		tv.tv_sec += usec / 1000000;

	}

	if	(offset_sec)
	{
		tv.tv_sec += offset_sec;
	}

	/* Formated output is simpler with \c snprintf and easier to
	 * read as output manipulators.
	 */

	struct tm t;
	memset(&t, 0, sizeof t);
	localtime_r(&tv.tv_sec, &t);

	snprintf(buf, sizeof buf, "%04d-%02d-%02d %02d:%02d:%02d.%06lu",
		t.tm_year + 1900,
		t.tm_mon + 1,
		t.tm_mday,
		t.tm_hour,
		t.tm_min,
		t.tm_sec,
		tv.tv_usec
	);

	return buf;
}
