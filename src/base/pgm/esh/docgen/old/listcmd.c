/*	Mischbefehle auflisten
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include "eisdoc.h"


/*	Mischbefehle auflisten
*/

void ShowMacro(Cmd_t *cmd, io_t *io)
{
	char *fmt;
	int last;

	fmt = cmd->par;
	last = '\n';

	if	(fmt == NULL)	return;

	for (last = '\n'; *fmt != 0; last = *fmt, fmt++)
	{
		if	(*fmt == '\n' || *fmt == '\t')
		{
			io_putc(*fmt, io);
		}
		else if	((unsigned char) *fmt < ' ')
		{
			io_printf(io, "^%c", (unsigned char) *fmt + '@');
		}
		else if	((unsigned char) *fmt >= 127)
		{
			io_printf(io, "\\%03o", (unsigned char) *fmt);
		}
		else	io_printf(io, "%c", *fmt);
	}

	if	(last != '\n')
		io_putc('\n', io);
}


static void list_cmd(io_t *io, Cmd_t *cmd)
{
	int n;

	n = io_printf(io, "%s[%d]\t", cmd->name, cmd->narg);

	if	(n < 9)	io_putc('\t', io);

	io_printf(io, "%s\n", cmd->desc ? cmd->desc : cmd->type->name);

	if	(cmd->type && cmd->type->show)
		cmd->type->show(cmd, io);
}


/*	Befehlstabelle auflisten
*/

void ListCmdTab (io_t *io, CmdTab_t *tab)
{
	Cmd_t *cmd;
	int i;

	if	(tab == NULL)	return;

	for (i = tab->buf.used, cmd = tab->buf.data; i > 0; i--, cmd++)
		list_cmd(io, cmd);
}
