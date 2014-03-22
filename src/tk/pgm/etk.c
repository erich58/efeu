/*
:*:	command interpreter with Tcl/Tk support
:de:	Befehlsinterpreter mit Tcl/Tk unterstütung

$Copyright (C) 2002 Erich Frühstück
This file is part of EFEU.

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

#include <EFEU/Resource.h>
#include <EFEU/CmdPar.h>
#include <EFEU/pconfig.h>
#include <EFEU/cmdeval.h>
#include <EFEU/preproc.h>
#include <EFEU/mdmat.h>
#include <EFEU/efutil.h>
#include <EFEU/database.h>
#include <EFEU/Random.h>
#include <EFEU/Pixmap.h>
#include <EFEU/Debug.h>
#include <Math/TimeSeries.h>
#include <Math/pnom.h>
#include <Math/mdmath.h>
#include <Math/func.h>
#include <GUI/ETK.h>

int main(int narg, char **arg)
{
	SetProgName(arg[0]);
	SetVersion(EFEU_VERSION);

	SetupStd();
	SetupUtil();
	SetupPreproc();

	SetupDataBase();
	SetupTimeSeries();
	SetupRandom();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();
	SetupMathFunc();
	SetupPixmap();
	SetupReadline();
	SetupDebug();
	SetupETK();

	return EshEval(&narg, arg);
}
