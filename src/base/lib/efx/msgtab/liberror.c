/*
Ausgabe einer Meldung

$Copyright (C) 1994 Erich Frühstück
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
#include <EFEU/procenv.h>
#include <EFEU/msgtab.h>
#include <EFEU/parsub.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/pconfig.h>

#define	ERRFMT	getmsg(MSG_EFEU, 2, "$!: ERROR $1.\n")


/*	Bibliotheksmeldung ausgeben
*/

void errmsg(const char *name, int num)
{
	char *fmt;

	fmt = getmsg(name, num, NULL);

	if	(fmt == NULL)
	{
		if	(name)
		{
			reg_fmt(1, "%s:%d", name, num);
		}
		else	reg_fmt(1, "%d", num);

		fmt = ERRFMT;
	}

	io_psub(ioerr, fmt);
}


void liberror(const char *name, int num)
{
	errmsg(name, num);
	exit(EXIT_FAILURE);
}
