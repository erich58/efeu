/*	EFEU-Informationssystem
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pconfig.h>
#include <EFEU/preproc.h>
#include "eistty.h"

#define	CONFIRM_QUIT	0

#define	ctrl(c)	((c) & 0x1f)

#define	PSIZE ((info_win->_maxy - 2) / 2)

int Lines = 0;
int CommandInfo = 0;
char *Node = NULL;

Var_t vardef[] = {
	{ "Lines",	&Type_int, &Lines },
	{ "Node",	&Type_str, &Node },
	{ "CommandInfo",	&Type_bool, &CommandInfo },
};

static int eval(int c);

static InfoNode_t *LoadCommand(InfoNode_t *info, const char *def)
{
	char *cmd = msprintf("%s -dump", def);
	io_t *io = io_popen(cmd, "r");

	info = AddInfo(info, def, NULL, NULL, NULL);
	IOLoadInfo(info, io);
	memfree(cmd);
	io_close(io);
	return info;
}

static InfoNode_t *load(InfoNode_t *info, const char *def)
{
	io_t *io;
	char *name;
	
	name = fsearch(InfoPath, NULL, def, NULL);

	if	(name == NULL)
		name = fsearch(InfoPath, NULL, def, "info");

	if	(name == NULL)
	{
		message("findopen", MSG_FTOOLS, 6, 1, def);
		procexit(EXIT_FAILURE);
	}

	io = io_fileopen(name, "rz");
	info = AddInfo(info, def, NULL, NULL, NULL);
	io = io_ptrpreproc(io, &InfoPath);
	IOLoadInfo(info, io);
	io_close(io);
	memfree(name);
	return info;
}

/*	Hauptprogramm
*/

int main(int narg, char **arg)
{
	int i;
	int key;
	InfoNode_t *info;

	libinit(arg[0]);
	SetupWin();
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

	info = GetInfo(NULL, NULL);
	info->list = NULL;
	info->par = NULL;
	info = NULL;

	for (i = 1; i < narg; i++)
	{
		if	(CommandInfo)
		{
			info = LoadCommand(NULL, arg[i]);
		}
		else	info = load(NULL, arg[i]);
	}

	if	(narg > 2)	info = NULL;
	
	InitWin();

	if	(Lines)
	{
		info_win = NewWindow(WindowSize(Lines, COLS, 0, 0, 0));
	}
	else	info_win = NewWindow(NULL);

	keypad(info_win, TRUE);
	scrollok(info_win, FALSE);
	leaveok(info_win, TRUE);
	ShowWindow(info_win);

	key = '\f';
	MakePart(GetInfo(info, Node));

	while (eval(key))
		key = wgetch(info_win);

	DelWindow(info_win);
	EndWin();
	libexit(EXIT_SUCCESS);
	return 0;
}

static int eval(int key)
{
	char *p;

	switch (key)
	{
	case '\f':
		wclear(info_win);
		wrefresh(info_win);
		break;
	case 'q':
#if	CONFIRM_QUIT
		wmove(info_win, LASTLINE, 0);
		wclrtobot(info_win);
		wattrset(info_win, 0);
		leaveok(info_win, FALSE);
		waddstr(info_win, "Programm beenden ? j\b");
		wrefresh(info_win);
		leaveok(info_win, TRUE);

		switch (wgetch(info_win))
		{
		case '\n':
		case '\r':
		case 'j':
		case 'J':
			return 0;
		default:
			break;
		}

		break;
#else
		return 0;
#endif
	case 'h':
	case KEY_HOME:
		GotoHome(); break;
	case 'e':
	case KEY_END:
		GotoEnd(); break;
	case 'f':
	case '\n':
	case '\r':
	case KEY_RIGHT:
		NextInfo(); break;
	case 'b':
	case KEY_LEFT:
		PrevInfo(); break;
	case 'd':
	case KEY_DOWN:
		MoveRef(1); break;
	case 'u':
	case KEY_UP:
		MoveRef(-1); break;
	case 'n':
	case KEY_NPAGE:
		MovePos(PSIZE); break;
	case 'p':
	case KEY_PPAGE:
		MovePos(-PSIZE); break;
	case ctrl('H'):
	case KEY_BREAK:
	case KEY_BACKSPACE:
	case 127:
	case 'k':
		MovePos(-1); break;
	case ' ':
	case 'j':
		MovePos(1); break;
	case '?':
		MakePart(load(NULL, "help"));
		break;
	case 't':
		MakePart(NULL);
		break;
	case ':':
		if	((p = GetString("Knoten: ")) != NULL)
		{
			MakePart(GetInfo(NULL, p));
			memfree(p);
		}
		break;
	case '<':
		if	((p = GetString("Datenfile: ")) != NULL)
		{
			WinMessage("%s -dump", p);
			MakePart(load(0, p));
			memfree(p);
		}

		break;
	case '|':
		if	((p = GetString("Kommando: ")) != NULL)
		{
			WinMessage("|%s -dump", p);
			MakePart(LoadCommand(NULL, p));
			memfree(p);
		}

		break;
	case '/':
		SearchKey(GetString("Suche: "));
		break;
	default:
		beep();
		return 1;
	}

	werase(info_win);
	ShowPart();
	wrefresh(info_win);
	return 1;
}
