/*	Befehlsinterpreter
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include <EFEU/cmdeval.h>
#include <EFEU/efutil.h>
#include <EFEU/oldpixmap.h>
#include <EFEU/Debug.h>
#include <EFEU/CmdPar.h>

int main (int narg, char **arg)
{
	SetProgName(arg[0]);
	SetVersion("$Id: pixmap.c,v 1.5 2002-11-13 06:18:50 ef Exp $");

	SetupStd();
	SetupUtil();
	SetupPreproc();
	SetupReadline();
	SetupOldPixMap();

	return EshEval(&narg, arg);
}
