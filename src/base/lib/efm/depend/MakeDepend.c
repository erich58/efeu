/*	Abhängigkeitslisten
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.6
*/

#include <EFEU/ftools.h>
#include <EFEU/MakeDepend.h>

#define	BREAK_COL	70	/* Spaltenpostion für Fortsetzungszeile */

int MakeDepend = 0;

VECBUF(TargetList, 16, sizeof(char *));
VECBUF(DependList, 16, sizeof(char *));

static int cmp_name (const char **a, const char **b)
{
	return mstrcmp(*a, *b);
}

static void add_name (vecbuf_t *tab, const char *name)
{
	char *p, **ptr;

	if	(name == NULL)	return;
	if	(*name == '|')	return;

	p = mstrcpy(name);
	ptr = vb_search(tab, &p, (comp_t) cmp_name, VB_REPLACE); 

	if	(ptr)	memfree(*ptr);
}

void AddTarget (const char *name)
{
	add_name(&TargetList, name);
}

void AddDepend (const char *name)
{
	add_name(&DependList, name);
}

static int put_name (io_t *io, int col, const char *name)
{
	if	(col)
	{
		io_putc(' ', io);
		col++;

		if	(name && col + strlen(name) >= BREAK_COL)
		{
			io_puts("\\\n ", io);
			col = 1;
		}
	}
			
	return col + io_puts(name, io);
}

void MakeDependRule (io_t *io)
{
	int n, p;
	char **ptr;

	io_putc('\n', io);
	p = 0;

	for (n = TargetList.used, ptr = TargetList.data; n-- > 0; ptr++)
		p = put_name(io, p, *ptr);

	if	(TargetList.used)
	{
		io_putc(':', io);
		p++;
	}

	for (n = DependList.used, ptr = DependList.data; n-- > 0; ptr++)
		p = put_name(io, p, *ptr);

	io_putc('\n', io);
}
