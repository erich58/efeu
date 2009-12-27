/*	Curses Schnittstelle
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pconfig.h>
#include <EFEU/efwin.h>

static int WinMode = 0;

extern void (*callproc_init) (void);
extern void (*callproc_exit) (void);

void (*callproc_init_base) (void);
void (*callproc_exit_base) (void);

static void callproc_init_win()
{
	EndWin();

	if	(callproc_init_base)
		callproc_init_base();
}

static void callproc_exit_win()
{
	if	(callproc_exit_base)
		callproc_exit_base();

	InitWin();
}

void EndWin(void)
{
	if	(WinMode == 1)
	{
		clear();
		move(0, 0);
		refresh();
		endwin();
	}

	WinMode = 2;
}

void InitWin(void)
{
	switch (WinMode)
	{
	case 0:	

		callproc_init_base = callproc_init;
		callproc_init = callproc_init_win;
		callproc_exit_base = callproc_exit;
		callproc_exit = callproc_exit_win;

		atexit(EndWin);
		initscr();
		raw();
		noecho();
		refresh();
		break;

	case 1:

		break;

	default:

		RefreshAll();
		doupdate();
		break;
	}

	WinMode = 1;
}


/*	Funktionstabelle
*/

/*
static Var_t vdef[] = {
	{ "MoName", &Type_str, MoName, tabsize(MoName) },
	{ "WoName", &Type_str, WoName, tabsize(WoName) },
};
*/

/*	Initialisieren
*/

void SetupWin(void)
{
/*
	AddVar(NULL, vdef, tabsize(vdef));
	AddFuncDef(fdef, tabsize(fdef));
*/
}
