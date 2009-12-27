/*	Tabelle mit Mischbefehlen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"

/*	Referenztype
*/

static void del_cmd (Cmd_t *cmd)
{
	memfree(cmd->name);
	memfree(cmd->desc);

	if	(cmd->type && cmd->type->clean)
		cmd->type->clean(cmd->par);
}

static void ct_clean (CmdTab_t *tab)
{
	int i;
	Cmd_t *cmd;

	for (i = tab->buf.used, cmd = tab->buf.data; i > 0; i--, cmd++)
		del_cmd(cmd);

	vb_free(&tab->buf);
	memfree(tab->name);
	memfree(tab);
}

static char *ct_ident (CmdTab_t *tab)
{
	return msprintf("%#s", tab ? tab->name : NULL);
}

reftype_t CmdTabType = { "CmdTab", (clean_t) ct_clean, 0, (ident_t) ct_ident };


CmdTab_t *NewCmdTab (const char *name)
{
	CmdTab_t *tab;

	tab = memalloc(sizeof(CmdTab_t));
	rd_init(tab, &CmdTabType);
	tab->name = mstrcpy(name);
	vb_init(&tab->buf, 16, sizeof(Cmd_t));
	return tab;
}


/*	Vergleichsfunktion
*/

static int cmd_cmp (const Cmd_t *a, const Cmd_t *b)
{
	return mstrcmp(a->name, b->name);
}


/*	Mischbefehl abfragen/löschen/erweitern
*/

Cmd_t *GetCmd (CmdTab_t *tab, const char *name)
{
	Cmd_t key;

	if	(tab == NULL)	return NULL;

	key.name = (char *) name;
	return vb_search(&tab->buf, &key, (comp_t) cmd_cmp, VB_SEARCH);
}

Cmd_t *AddCmd (CmdTab_t *tab, const char *name)
{
	Cmd_t key, *ptr;

	if	(tab == NULL || name == NULL)	return NULL;

	memset(&key, 0, sizeof(Cmd_t));
	key.name = mstrcpy(name);
	ptr = vb_search(&tab->buf, &key, (comp_t) cmd_cmp, VB_ENTER);
	memfree(key.name);
	return ptr;
}

void DelCmd (CmdTab_t *tab, const char *name)
{
	Cmd_t key, *ptr;

	if	(tab == NULL)	return;

	key.name = (char *) name;
	ptr = vb_search(&tab->buf, &key, (comp_t) cmd_cmp, VB_DELETE);

	if	(ptr)
		del_cmd(ptr);
}
