/*
:*:	administration of test parameters
:de:	Verwaltung von Testparameter

$Copyright (C) 2003, 2005 Erich Frühstück
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

#include <EFEU/TestDef.h>

void *CreateTestPar (TestParDef *def, const char *opt, const char *arg)
{
	return def ? def->create(opt, arg, def->data) : NULL;
}

void CleanTestPar (TestParDef *def, void *par)
{
	if	(def && par)
		def->clean(par);
}
