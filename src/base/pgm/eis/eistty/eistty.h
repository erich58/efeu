/*	EFEU-Informationssystem
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
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
