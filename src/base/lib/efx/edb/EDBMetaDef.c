/*	EDB-Metafiledefinitionen
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDBMeta.h>
#include <EFEU/nkt.h>
#include <EFEU/Resource.h>

#define	INFO_NAME	"meta"

#define	INFO_HEAD	\
	":*:meta data specification" \
	":de:Metadefinitionen"

static NameKeyTab MetaDef = NKT_DATA("EDBMetaDef", 60, NULL);

void AddEDBMetaDef (EDBMetaDef *def, size_t dim)
{
	SetupEDBMeta();

	for (; dim-- > 0; def++)
		nkt_insert(&MetaDef, def->name, def);
}

EDBMetaDef *GetEDBMetaDef (const char *name)
{
	SetupEDBMeta();
	return nkt_fetch(&MetaDef, name, NULL);
}

static int do_list (const char *name, void *data, void *par)
{
	EDBMetaDef *mdef;
	IO *out;
	char *fmt;
	IO *desc;
	int c;


	mdef = data;
	out = par;
	fmt = GetFormat(mdef->desc);
	desc = langfilter(io_cstr(fmt), NULL);

	io_puts(mdef->name, out);
	io_putc('\t', out);

	while  ((c = io_getc(desc)) != EOF)
	{
		if	(c == '$')
		{
			switch (c = io_getc(desc))
			{
			case '1':	io_puts(mdef->name, out); break;
			default:	io_putc(c, out); break;
			}
		}
		else	io_putc(c, out);
	}

	io_close(desc);
	io_putc('\n', out);
	return 0;
}

void ListEDBMetaDef (IO *out)
{
	SetupEDBMeta();
	nkt_walk(&MetaDef, do_list, out);
}

static void meta_info (IO *out, InfoNode *info)
{
	ListEDBMetaDef(out);
}

void EDBMetaInfo (InfoNode *info)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	AddInfo(info, INFO_NAME, INFO_HEAD, meta_info, NULL);
}
