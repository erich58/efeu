/*	EDB-Ausgabedefinitionen
	$Copyright (C) 2004 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>
#include <EFEU/nkt.h>
#include <EFEU/Resource.h>

#define	INFO_NAME	"print"

#define	INFO_HEAD	\
	":*:output specification" \
	":de:Ausgabedefinitionen"

static NameKeyTab PrintDef = NKT_DATA("EDBPrintDef", 60, NULL);

void AddEDBPrintDef (EDBPrintDef *def, size_t dim)
{
	SetupEDBPrint();

	for (; dim-- > 0; def++)
		nkt_insert(&PrintDef, def->name, def);
}

EDBPrintDef *GetEDBPrintDef (const char *name)
{
	SetupEDBPrint();
	return nkt_fetch(&PrintDef, name, NULL);
}

static int do_list (const char *name, void *data, void *par)
{
	EDBPrintDef *pdef;
	IO *out;
	char *fmt;
	IO *desc;
	int c;


	pdef = data;
	out = par;
	fmt = GetFormat(pdef->desc);
	desc = langfilter(io_cstr(fmt), NULL);

	io_puts(pdef->name, out);
	io_putc('\t', out);

	while  ((c = io_getc(desc)) != EOF)
	{
		if	(c == '$')
		{
			switch (c = io_getc(desc))
			{
			case '1':	io_puts(pdef->name, out); break;
			case '2':	io_puts(pdef->par, out); break;
			default:	io_putc(c, out); break;
			}
		}
		else	io_putc(c, out);
	}

	io_close(desc);
	io_putc('\n', out);
	return 0;
}

void ListEDBPrintDef (IO *out)
{
	SetupEDBPrint();
	nkt_walk(&PrintDef, do_list, out);
}

static void print_info (IO *out, InfoNode *info)
{
	ListEDBPrintDef(out);
}

void EDBPrintInfo (InfoNode *info)
{
	static int setup_done = 0;

	if	(setup_done)	return;

	setup_done = 1;
	AddInfo(info, INFO_NAME, INFO_HEAD, print_info, NULL);
}
