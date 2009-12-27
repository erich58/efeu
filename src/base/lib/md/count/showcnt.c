/*	Auflisten von Zählerdefinitionen
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

#define	LBL_COUNT \
	":*:counter types" \
	":de:Zählerdefinitionen"

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
	AddInfo(count_root, entry->name, entry->desc,
		print_count, entry);
	return 1;
}

void MdCountInfo (InfoNode_t *info, xtab_t *tab)
{
	count_root = AddInfo(info, "count", LBL_COUNT, NULL, NULL);
	xwalk(tab, (visit_t) add_count);
}
