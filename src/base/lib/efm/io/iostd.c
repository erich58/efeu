/*
Standard Ein-Ausgabe (Plain)

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

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

/*	Lesen von Standardeingabe
*/

static int std_get (void *ptr)
{
	return getchar();
}


/*	Zeichen nach Standardausgabe schreiben
*/

static int std_put (int c, void *ptr)
{
	return putchar(c);
}


/*	Zeichen nach Standardfehler schreiben
*/

static int err_put (int c, void *ptr)
{
	return putc(c, stderr);
}

static int std_ctrl (void *ptr, int c, va_list list)
{
	switch (c)
	{
	case IO_FLUSH:	return fflush(stdout);
	case IO_CLOSE:	return 0;
	case IO_IDENT:	*va_arg(list, char **) = ptr; return 0;
	default:	return EOF;
	}
}

static int err_ctrl (void *ptr, int c, va_list list)
{
	switch (c)
	{
	case IO_FLUSH:	return fflush(stderr);
	case IO_CLOSE:	return 0;
	case IO_IDENT:	*va_arg(list, char **) = ptr; return 0;
	default:	return EOF;
	}
}

static IO ios_batch = STD_IODATA(std_get, std_put,
	std_ctrl, "<stdin,stdout>");

static IO ios_err = STD_IODATA(NULL, err_put,
	err_ctrl, "<stderr>");

IO *io_batch (void)
{
	return rd_refer(&ios_batch);
}

IO *iostd = &ios_batch;
IO *iomsg = &ios_batch;
IO *ioerr = &ios_err;

