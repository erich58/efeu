/*
:*:	convert resource file intpo xml-Format
:de:	Resourcedatei in XML-Format umwandeln

$Copyright (C) 2008 Erich Frühstück
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

int main (int argc, char **argv)
{
	SetProgName(argv[0]);
	SetVersion("$Id: res2xml.c,v 1.1 2008-07-17 03:48:39 ef Exp $");
	ParseCommand(&argc, argv);
	return EXIT_SUCCESS;
}
