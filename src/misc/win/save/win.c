/*	Testprogramm für Curses
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <pconfig.h>
#include <efwin.h>
#include <term.h>

static char *Input = NULL;

static Var_t vardef[] = {
	{ "Input", &Type_str, &Input },
};


/*	Hauptprogramm
*/

io_t *wio = NULL;

int main (int narg, char **arg)
{
	WINDOW *bg;

	libinit(arg[0]);
	SetupRand48();
	SetupWin();
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	InitWin();
	bg = StippleBackground();
	wio = io_winopen(WindowSize(20, 30, W_TOP|W_LEFT|W_FRAME, 0, 0));
	WinTrace(0, 0);
	io_close(wio);
	/*
	system("sleep 1");
	*/
	DelWindow(bg);
	EndWin();
}
