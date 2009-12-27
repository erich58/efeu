/*	Kleine Differenzen in TeX-Dokumenten markieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/pconfig.h>
#include <EFEU/efutil.h>

char *Name1 = NULL;	/* 1. Eingabefile */
char *Name2 = NULL;	/* 2. Eingabefile */
char *Output = NULL;	/* Ausgabefile */
char *Log = NULL;	/* Differenzprotokoll */

Var_t vardef[] = {
	{ "Name1",	&Type_str, &Name1 },
	{ "Name2",	&Type_str, &Name2 },
	{ "Output",	&Type_str, &Output },
	{ "Log",	&Type_str, &Log },
};

#define	WHITE(c)	((c) == 0 || (c) == '\n' || (c) == ' ' || (c) == '\t')

static char *get_token(strbuf_t *sb, io_t *io)
{
	int c;

	sb_clear(sb);

	while ((c = io_getc(io)) != EOF)
	{
		sb_putc(c, sb);

		if	(WHITE(c))
			break;
	}

	if	(sb->pos == 0)	return NULL;

	sb_putc(0, sb);
	return sb->data;
}

static void put_token(char *p, io_t *io)
{
	io_putc('{', io);

	while (*p != 0)
	{
		if	(WHITE(*p))
			io_putc('}', io);

		io_putc(*p, io);
		p++;
	}
}

/*	Hauptprogramm
*/

int main(int narg, char **arg)
{
	io_t *ein1, *ein2, *aus, *log;
	strbuf_t *sb1, *sb2;
	char *p1, *p2;

	libinit(arg[0]);
	pconfig(NULL, vardef, tabsize(vardef));
	loadarg(&narg, arg);

/*	Datenfile verarbeiten
*/
	ein1 = io_fileopen(Name1, "r");
	ein2 = io_fileopen(Name2, "r");
	aus = io_fileopen(Output, "w");
	log = Log ? io_fileopen(Log, "w") : NULL;
	sb1 = new_strbuf(1024);
	sb2 = new_strbuf(1024);

	while ((p1 = get_token(sb1, ein1)))
	{
		if	(WHITE(p1[0]))
		{
			io_puts(p1, aus);
			continue;
		}

		while ((p2 = get_token(sb2, ein2)))
			if (!WHITE(p2[0])) break;

		if	(p2 == NULL)	break;

		if	(strcmp(p1, p2) != 0)
		{
			if	(log)
			{
				io_puts(p1, log);
				io_putc('\t', log);
				io_puts(p2, log);
				io_putc('\n', log);
			}

			io_puts("\\DIFF", aus);
			put_token(p1, aus);
			put_token(p2, aus);
		}
		else	io_puts(p1, aus);
	}

	while (p1)
	{
		io_puts(p1, aus);
		p1 = get_token(sb1, ein1);
	}

	io_close(ein1);
	io_close(ein2);
	io_close(aus);
	return 0;
}
