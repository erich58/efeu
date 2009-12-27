/*
EFEU-Informationssystem

$Copyright (C) 1998 Erich Fr�hst�ck
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

#ifndef	_EISTTY_H
#define	_EISTTY_H	1

#include <EFEU/Info.h>
#include <EFEU/efwin.h>

extern WINDOW *info_win;
extern InfoNode_t *info_node;

#define	LASTLINE 	(info_win->_maxy - 1)

typedef struct {
	int y;		/* Zeilenposition */
	int x;		/* Spaltenposition */
	int att;	/* Attribut */
	char *str;	/* Zeichenkette */
	InfoNode_t *info;	/* Verzweigungsknoten */
} InfoPart_t;

void MakePart (InfoNode_t *info);
void ShowPart (void);

void MovePos (int val);
void MoveRef (int val);
void GotoHome (void);
void GotoEnd (void);
int SearchKey (char *key);
void PrevInfo (void);
void NextInfo (void);

void HelpWindow ();
char *GetString (char *prompt);
void WinMessage (char *fmt, ...);

#endif	/* _EISTTY_H */
