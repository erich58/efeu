/*
:*: system call
:de: Systemaufruf

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

#include <EFEU/procenv.h>

/*
:*:
The function |$1| is a wrapper around |system| with the following
difference: If |$1| is called with |NULL| as argument and
the global variable |Shell| is not |NULL|, |system(Shell)| is executed.
If |Shell| is not definied, |callproc| returns -1.
:de:
Die Funktion |$1| ist eine Hülle um |system| mit dem folgenden
Unterschied: Falls |$1| mit einem Nullpointer als Argument aufgerufen wird
und die Variable |Shell| ist nicht |NULL|, wird |system(Shell)| ausgeführt.
Falls |Shell| nicht definiert ist, liefert |callproc| -1.
*/

int callproc (const char *cmd)
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

/*
:*: If |$1| is true, no system call is performed.
:de: Falls |$1| verschieden von 0 ist, werden keine Systemaufrufe
durchgeführt.
*/
int callproc_lock = 0;

/*
:*: If |$1| is not |NULL|, |$1| is called before any system call.
:de: Falls |$1| verschieden von |NULL| ist, wird |$1| vor jedem
Systemaufruf aufgerufen.
*/
void (*callproc_init) (void) = NULL;

/*
:*: If |$1| is not |NULL|, |$1| is called after any system call.
:de: Falls |$1| verschieden von |NULL| ist, wird |$1| nach jedem
Systemaufruf aufgerufen.
*/
void (*callproc_exit) (void) = NULL;
