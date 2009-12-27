/*
Curses-Schnittstelle

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_efwin_h
#define	EFEU_efwin_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <curses.h>
#include <term.h>

#ifndef	MSG_EFWIN
#define	MSG_EFWIN	"efwin"	/* Name für Fehlermeldungen */
#endif

#ifndef	KEY_ESC
#define	KEY_ESC	0x1b
#endif

/*	Steuerflags
*/

#define	W_CENTER	0
#define	W_LEFT		0x1
#define	W_RIGHT		0x2
#define	W_TOP		0x4
#define	W_BOT		0x8
#define	W_FRAME		0x10

typedef struct {
	int height;	/* Fensterhöhe */
	int width;	/* Fensterbreite */
	int flags;	/* Fensterflags */
	int y;		/* Y - Koordinate */
	int x;		/* X - Koordinate */
} WinSize;

WinSize *WindowSize (int height, int width, int pos, int y0, int x0);
WinSize *CurrentPos (WINDOW *win, int height, int width, int pos);
WinSize *CenterPos (int height, int width);
WinSize *Rand48Pos (int height, int width);
WinSize *CheckSize (WinSize *wp);
WinSize *Frame (WinSize *wp);

WINDOW *NewWindow (WinSize *wp);
WINDOW *FrameWindow (WinSize *wp);
void DelWindow (WINDOW *win);
void EndWin(void);
void InitWin(void);
void SetupWin(void);
void WinInfo (const char *name);

void ShowWindow (WINDOW *win);
void HideWindow (WINDOW *win);
void RefreshAll (void);

WINDOW *StippleBackground (void);

IO *io_winopen (WinSize *ws);
IO *io_winout (WINDOW *win);

void WinConfirm (WinSize *ws, char *label);
int WinQuestion (WinSize *ws, char *label, char *s1, char *s2);
char *WinSelect (WinSize *ws, char **list, int dim, int cols);
char *WinEdit (WinSize *ws, char *def);

typedef struct {
	int key;
	char *label;
	void *data;
} WinMenu;

void *WinMenu_create (WinSize *ws, WinMenu *list, int dim, int cols);

void WinTrace (int height, int width);
void WinSystem (const char *cmd);

void DrawBox (WINDOW *win);
void OverlayWindow (WINDOW *child, WINDOW *parent);

#ifndef	getmaxx
#define getmaxx(win)		((win)?((win)->_maxx + 1):ERR)
#endif

#ifndef	getmaxy
#define getmaxy(win)		((win)?((win)->_maxy + 1):ERR)
#endif

#endif	/* EFEU/efwin.h */
