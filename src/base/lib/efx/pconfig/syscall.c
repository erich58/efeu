/*
Systemaufruf

$Copyright (C) 1997 Erich Frühstück
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

#include <EFEU/pconfig.h>

void (*callproc_init) (void) = NULL;
void (*callproc_exit) (void) = NULL;
int callproc_lock = 0;

int callproc(const char *cmd)
{
	extern char *Shell;
	int n;

	if	(callproc_lock)	return -1;

	if	(callproc_init)
		callproc_init();

	if	(cmd != NULL)	n = system(cmd) >> 8;
	else if	(Shell != NULL)	n = system(Shell) >> 8;
	else			n = -1;

	if	(callproc_exit)
		callproc_exit();

	return n;
}
