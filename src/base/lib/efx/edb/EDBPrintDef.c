/*	EDB-Ausgabedefinitionen
	$Copyright (C) 2004 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDB.h>
#include <EFEU/nkt.h>
#include <EFEU/Resource.h>
#include <EFEU/parsearg.h>

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

static void pdef_psub (EDBPrintDef *pdef, int c, IO *out)
{
	switch (c)
	{
	case '1':

		io_puts(pdef->name, out);
		break;

	case '2':

		if	(pdef->syntax)
		{
			AssignArg *x = assignarg(pdef->syntax, NULL, NULL);

			io_puts(EDBPrintDef_psub(pdef->par, x->opt, x->arg),
				out);
			memfree(x);
		}
		else	io_puts(pdef->par, out);

		break;

	default:

		io_putc(c, out);
		break;
	}
}

void ShowEDBPrintDef (IO *out, EDBPrintDef *pdef, int verbosity)
{
	char *fmt;
	IO *desc;
	int c;

	fmt = GetFormat(pdef->desc);
	desc = langfilter(io_cstr(fmt), NULL);

	if	(!verbosity)
	{
		io_puts(pdef->name, out);
		io_putc('\t', out);
	}

	while  ((c = io_getc(desc)) != EOF && c != '\n')
	{
		if	(c == '$')
		{
			pdef_psub(pdef, io_getc(desc), out);
		}
		else	io_putc(c, out);
	}

	io_putc('\n', out);

	if	(verbosity)
	{
		char *p = GetFormat(":en:Syntax:de:Verwendung");

		io_putc('\n', out);
		io_langputs(p, out);

		if	(p)
			io_puts(": ", out);

		io_puts(pdef->name, out);
		io_puts(pdef->syntax, out);

		if	(io_peek(desc) != EOF)
			io_puts("\n\n", out);

		while  ((c = io_getc(desc)) != EOF)
		{
			if	(c == '$')
			{
				pdef_psub(pdef, io_getc(desc), out);
			}
			else	io_putc(c, out);
		}

		io_putc('\n', out);
	}

	io_close(desc);
}

static int do_list (const char *name, void *data, void *par)
{
	ShowEDBPrintDef(par, data, 0);
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
