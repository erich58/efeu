/*	Befehl abfragen
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include "eisdoc.h"

/*	Tabellenstack
*/

typedef struct CmdTabStack_s CmdTabStack_t;

struct CmdTabStack_s {
	CmdTabStack_t *next;	/* Nächster Eintrag */
	CmdTab_t *tab;		/* Lokale Tabelle */
};

static ALLOCTAB(StackBuf, 8, sizeof(CmdTabStack_t));

static CmdTabStack_t *CmdTabStack = NULL;
static CmdTab_t *localtab = NULL;

void PushCmdTab (CmdTab_t *tab)
{
	CmdTabStack_t *x = CmdTabStack;
	CmdTabStack = new_data(&StackBuf);
	CmdTabStack->next = x;
	CmdTabStack->tab = localtab;
	localtab = tab;
}


CmdTab_t *PopCmdTab(void)
{
	if	(CmdTabStack)
	{
		CmdTab_t *tab = localtab;
		CmdTabStack_t *x = CmdTabStack;
		CmdTabStack = x->next;
		localtab = x->tab;
		del_data(&StackBuf, x);
		return tab;
	}
	else	return NULL;
}


/*	Mischbefehl abfragen
*/

Cmd_t *GetMergeCmd (const char *name)
{
	Cmd_t *cmd;
	CmdTabStack_t *x;

	if	((cmd = GetCmd(builtin, name)) != NULL)
		return cmd;

	if	((cmd = GetCmd(localtab, name)) != NULL)
		return cmd;

	for (x = CmdTabStack; x && !cmd; x = x->next)
		cmd = GetCmd(x->tab, name);

	return cmd;
}


void ListMergeCmd (io_t *io)
{
	CmdTabStack_t *x;

	io_nputc('-', io, 70);
	io_putc('\n', io);
	ListCmdTab(io, builtin);
	io_nputc('-', io, 70);
	io_putc('\n', io);
	ListCmdTab(io, localtab);

	for (x = CmdTabStack; x != NULL; x = x->next)
	{
		io_nputc('-', io, 70);
		io_putc('\n', io);
		ListCmdTab(io, x->tab);
	}

	io_nputc('-', io, 70);
	io_putc('\n', io);
}


Cmd_t *AddMergeCmd (const char *name)
{
	if	(localtab == NULL)
		localtab = NewCmdTab(NULL);

	return AddCmd(localtab, name);
}

void DelMergeCmd (const char *name)
{
	if	(localtab)
		DelCmd(localtab, name);
}
