/*	Auflisten von Zählerdefinitionen
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

void md_showcnt(io_t *io, xtab_t *tab)
{
	int i;
	MdCount_t *cnt;

	for (i = 0; i < tab->dim; i++)
	{
		cnt = tab->tab[i];
		io_puts(cnt->name, io);
		io_putc('\t', io);
		/*
		io_puts(cnt->type, io);
		io_putc('\t', io);
		*/
		io_puts(cnt->desc, io);
		io_putc('\n', io);
	}
}


static void print_count(io_t *io, InfoNode_t *info)
{
	MdCount_t *cnt = info->par;
	/*
	ListType(io, mdtype(cnt->type));
	*/
	io_puts(cnt->type, io);
	io_putc('\n', io);
}

static InfoNode_t *count_root = NULL;

static int add_count(MdCount_t *entry)
{
	AddInfo(count_root, entry->name, mstrcpy(entry->desc),
		print_count, entry);
	return 1;
}

void MdCountInfo (InfoNode_t *info, xtab_t *tab)
{
	count_root = AddInfo(info, "count", "Zählerdefinitionen", NULL, NULL);
	xwalk(tab, (visit_t) add_count);
}
