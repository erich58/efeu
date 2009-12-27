/*	Variablentabelle ausgeben
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <ctype.h>

#define	MAX_POS	79

int ShowVarTab(io_t *io, const char *pfx, VarTab_t *vtab)
{
	int i, n, k, pos;
	char *name;

	if	(vtab == NULL)	return io_puts("NULL", io);

	if	(pfx)		n = io_printf(io, "%s", pfx);
	else if	(vtab->name)	n = io_printf(io, "%s", vtab->name);
	else			n = io_printf(io, "%#p", vtab);

	n += io_puts(" = {", io);
	pos = MAX_POS;

	for (i = 0; i < vtab->tab.dim; i++)
	{
		name = ((Var_t *) vtab->tab.tab[i])->name;

		if	(name == NULL)	continue;

		if	(pos + strlen(name) > MAX_POS)
		{
			n += io_puts("\n\t", io);
			pos = 8;
			k = 0;
		}
		else	k = io_puts(" ", io);

		k += io_puts(name, io);
		pos += k;
		n += k;
	}

	n += io_puts("\n}", io);
	return n;
}
