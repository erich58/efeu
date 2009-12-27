/*	Befehlsinterpreter
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/pconfig.h>
#include <EFEU/cmdeval.h>
#include <EFEU/preproc.h>
#include <EFEU/mdmat.h>
#include <EFEU/efutil.h>
#include <EFEU/database.h>
#include <EFEU/Random.h>
#include <EFEU/Pixmap.h>
#include <Math/TimeSeries.h>
#include <Math/pnom.h>
#include <Math/mdmath.h>

#define	NAME	"esh"
#define	STDDEF	"eshdef"
#define	HNAME	"~/.esh_history"

static char *Input = NULL;	/* Eingabefile */
static char *CmdType = NULL;	/* Filetype für Skriptfiles */
static int PPOnly = 0;		/* Nur Preprozessor verwenden */


Var_t vardef[] = {
	{ "CmdType",	&Type_str, &CmdType },
	{ "PPOnly",	&Type_bool, &PPOnly },
};

Var_t svardef[] = {
	{ "Input",	&Type_str, &Input },
};

int main(int narg, char **arg)
{
	io_t *in;

	libinit(arg[0]);
	SetupUtil();
	SetupPreproc();
	SetupDataBase();
	SetupTimeSeries();
	SetupRand48();
	SetupRandom();
	SetupMdMat();
	SetupMath();
	SetupMdMath();
	SetupPnom();
	SetupSC();
	SetupPixmap();
	SetupReadline();
	SetupDebug();
	pconfig(NAME, vardef, tabsize(vardef));

	if	(!EshConfig(&narg, arg))
	{
		pconfig(STDDEF, svardef, tabsize(svardef));
		loadarg(&narg, arg);
		in = Input ? io_fileopen(Input, "r") : io_interact(NULL, HNAME);
	}
	else
	{
		char *fname = fsearch(IncPath, NULL, arg[0], CmdType);
		in = io_fileopen(fname ? fname : arg[0], "r");
		memfree(fname);
	}

	in = io_cmdpreproc(in);

	if	(PPOnly)
	{
		int c;

		while ((c = io_getc(in)) != EOF)
			io_putc(c, iomsg);
	}
	else	CmdEval(in, iomsg);

	io_close(in);
	return 0;
}
