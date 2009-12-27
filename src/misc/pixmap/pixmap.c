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

static char *Input = NULL;	/* Eingabefile */

#define	HNAME	".pix_history"

Var_t vardef[] = {
	{ "Input",	&Type_str, &Input },
};


int main(int narg, char **arg)
{
	io_t *in;

	libinit(arg[0]);
	SetupUtil();
	SetupPreproc();
	SetupOldPixMap();

	if	(!EshConfig(&narg, arg))
	{
		SetupReadline();
		pconfig(NULL, vardef, tabsize(vardef));
		loadarg(&narg, arg);
		in = Input ? io_fileopen(Input, "r") : io_interact(NULL, HNAME);
	}
	else if	((in = io_fileopen(arg[0], "r")) == NULL)
	{
		reg_set(1, arg[0]);
		liberror(NULL, 1);
	}

	in = io_cmdpreproc(in);
	CmdEval(in, iomsg);
	io_close(in);
	return 0;
}
