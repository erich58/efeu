/*	Informationseintrag suchen
	(c) 1998 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>
#include <ctype.h>

#define	INFO_BSIZE	16

static InfoNode_t root = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

static int cmp_info(const InfoNode_t **a, const InfoNode_t **b)
{
	return mstrcmp((*a)->name, (*b)->name);
}

static InfoNode_t *get_info (InfoNode_t *base, const char *name, char **nptr)
{
	char *ptr;
	InfoNode_t ibuf, *info, **ip;

	if	(!base || (name && *name == INFO_SEP))
		base = &root;

	if	(base->load)
		base->load(base);

	ptr = (char *) name;

	for (*nptr = ptr; ptr != NULL; *nptr = ptr)
	{
		ibuf.name = InfoNameToken(&ptr);

		if	(ibuf.name == NULL)
		{
			base = &root;
			continue;
		}

		if	(ibuf.name[0] == '.')
		{
			if	(ibuf.name[1] == 0)
			{
				memfree(ibuf.name);
				continue;
			}

			if	(ibuf.name[1] == '.' && ibuf.name[2] == 0)
			{
				if	(base->prev)	base = base->prev;

				memfree(ibuf.name);
				continue;
			}
		}

		info = &ibuf;
		ip = vb_search(base->list, &info, (comp_t) cmp_info, VB_SEARCH);
		memfree(ibuf.name);

		if	(ip)
		{
			base = *ip;

			if	(base->load)
				base->load(base);
		}
		else	break;
	}

	return base;
}

InfoNode_t *GetInfo (InfoNode_t *base, const char *name)
{
	char *ptr;
	InfoNode_t *info;

	info = get_info(base, name, &ptr);

	if	(ptr)
	{
		io_putc(INFO_KEY, ioerr);
		InfoName(ioerr, NULL, info);
		io_printf(ioerr, ": Eintrag %s nicht definiert\n", ptr);
		return NULL;
	}

	return info;
}

InfoNode_t *AddInfo (InfoNode_t *base, const char *name, char *label,
	PrintInfo_t func, void *par)
{
	InfoNode_t *info, **ip;
	char *ptr;

	base = get_info(base, name, &ptr);

	if	(ptr == NULL)
	{
		fprintf(stderr, "Knoten %s in Verwendung.\n", name);
		return NULL;
	}

	while (ptr != NULL)
	{
		if	(base->list == NULL)
		{
			base->list = memalloc(sizeof(vecbuf_t));
			vb_init(base->list, INFO_BSIZE, sizeof(InfoNode_t *));
		}

		info = NewInfo(base, InfoNameToken(&ptr));
		ip = vb_search(base->list, &info, (comp_t) cmp_info, VB_ENTER);
		base = *ip;
	}

	base->label = label;
	base->func = func;
	base->par = par;
	return base;
}
