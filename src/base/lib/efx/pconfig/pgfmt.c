/*	Filter fuer Seitenformatierung
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6

	Version 0.4
*/

#include <EFEU/pconfig.h>
#include <EFEU/stack.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

#define	DEF_MARGIN	7	/* Linker Rand */
#define	DEF_TABSIZE	5	/* Standardtabulator */
#define	DEF_MINCOL	40	/* Minalbreite für angepaßten Umbruch */
#define	DEF_WPMARGIN	72	/* Umbruchrand */
#define	DEF_BUFSIZE	64	/* Basisgroesse fuer Zwischenbuffer */
#define	DEF_COLUMNS	80	/* Standardspaltenbreite */

#define	CTRL_ROMAN	1	/* Roman - Font */
#define	CTRL_BOLD	2	/* Bold - Font */
#define	CTRL_ITALIC	3	/* Italic - Font */


/*	Eingabestruktur
*/

typedef struct PGFMT_S {
	io_t *io;		/* Eingabestruktur */
	io_t *tmp;		/* Zwischenspeicher */
	int save;		/* Zahl der gesicherten Zeichen */
	int cs_save;		/* gesicherte Zeichenlaenge */
	int cs_current;		/* aktuelle Zeichenlaenge */
	int leftmargin;		/* Linker Rand */
	int wpmargin;		/* Umbruchrand */
	int tabsize;		/* Tabulatorgroesse */
	int indent;		/* Aktueller Einzug */
	int save_indent;	/* Einzug fuer naechste Zeile */
	stack_t *ind_stack;	/* Stack mit Einrueckungen */
	int xpos;		/* Aktuelle Spaltenposition */
	int ypos;		/* Aktuelle Zeilenposition */
	stack_t *word;		/* Wordstack */
	int nl_flag;		/* Flag für Zeilenvorschub */
	char *hyphen;		/* Trennzeichen bei Zeilenende */
	int nofill;		/* Flag fuer Auffuellung */
	int TP_flag;		/* Flag fuer Vortext */
	int att_flag;		/* Flag füer Attributumwandlung */
	int (*put) (int c, struct PGFMT_S *filter); 
	int (*save_put) (int c, struct PGFMT_S *filter); 
} PGFMT;

static int pgf_ctrl (PGFMT *ptr, int req, va_list list);
static int pgf_put (int c, PGFMT *ptr);

static void restore_put (PGFMT *io);
static void savechar (int c, PGFMT *io);
static char *save_str (const char *str, PGFMT *pgf, int flag);
static void saveatt (const char *att, PGFMT *pgf);

static int ignore_put (int c, PGFMT *io);
static int txt_put (int c, PGFMT *io);
static int arg_put (int c, PGFMT *io);
static int esc_put (int c, PGFMT *io);
static int xesc_put (int c, PGFMT *io);
static int att_put (int c, PGFMT *io);
static void pgf_print (PGFMT *io, int flag);
static void pgf_save (PGFMT *io);
static void pgf_newline (PGFMT *io);
static int std_val (const char *arg, int val);

static void do_puts (char *str, io_t *io);

static void set_indent (PGFMT *filter);

static void cmd_eval (PGFMT *io, const char *cmd);

static void c_SH (PGFMT *io, const char *cmd, int par);
static void c_HP (PGFMT *io, const char *cmd, int par);
static void c_PP (PGFMT *io, const char *cmd, int par);
static void c_RS (PGFMT *io, const char *cmd, int par);
static void c_RE (PGFMT *io, const char *cmd, int par);
static void c_TP (PGFMT *io, const char *cmd, int par);
static void c_IP (PGFMT *io, const char *cmd, int par);
static void c_RBI (PGFMT *io, const char *cmd, int par);
static void c_RBI2 (PGFMT *io, const char *cmd, int par);
static void c_nf (PGFMT *io, const char *cmd, int par);
static void c_br (PGFMT *io, const char *cmd, int par);
static void c_nr (PGFMT *io, const char *cmd, int par);


typedef struct {
	char *name;
	void (*eval) (PGFMT *filter, const char *arg, int par);
	int par;
} CMD_DEF;


static CMD_DEF cmd_def[] = {
	{ "SH",	c_SH, 0 },
	{ "SS",	c_SH, 4 },
	{ "PP",	c_PP, 0 },
	{ "HP",	c_HP, 0 },
	{ "RS",	c_RS, 0 },
	{ "RE",	c_RE, 0 },
	{ "IP",	c_IP, 0 },
	{ "TP",	c_TP, 0 },
	{ "RI",	c_RBI2, ('R' << 8) + 'I' },
	{ "RB",	c_RBI2, ('R' << 8) + 'B' },
	{ "BR",	c_RBI2, ('B' << 8) + 'R' },
	{ "BI",	c_RBI2, ('B' << 8) + 'I' },
	{ "IR",	c_RBI2, ('I' << 8) + 'R' },
	{ "IB",	c_RBI2, ('I' << 8) + 'B' },
	{ "R",	c_RBI, 'R' },
	{ "B",	c_RBI, 'B' },
	{ "I",	c_RBI, 'I' },
	{ "nf",	c_nf, 1 },
	{ "fi",	c_nf, 0 },
	{ "br",	c_br, 0 },
	{ "nr",	c_nr, 0 },
};


io_t *io_pgfmt(io_t *io)
{
	if	(io != NULL)
	{
		PGFMT *pgf;
		int n;

		pgf = ALLOC(1, PGFMT);
		memset(pgf, 0, sizeof(PGFMT));
		pgf->io = io;
		pgf->put = txt_put;
		pgf->nl_flag = 1;
		pgf->tmp = io_tmpbuf(DEF_BUFSIZE);
		set_indent(pgf);
		pgf->leftmargin = DEF_MARGIN;
		pgf->tabsize = DEF_TABSIZE;

		n = io_ctrl(io, IO_COLUMNS);

		if	(n >= DEF_MINCOL)
		{
			pgf->wpmargin = n - DEF_MARGIN - 1;
		}
		else	pgf->wpmargin = DEF_WPMARGIN;
		
		io = io_alloc();
		io->put = (io_put_t) pgf_put;
		io->ctrl = (io_ctrl_t) pgf_ctrl;
		io->data = pgf;
		return io;
	}
	else	return NULL;
}


int pgf_put(int c, PGFMT *ptr)
{
	return ptr->put(c, ptr);
}


/*	Ausgabefunktion restaurieren
*/

void restore_put(PGFMT *filter)
{
	if	(filter->save_put)
	{
		filter->put = filter->save_put;
		filter->save_put = NULL;
	}
	else	filter->put = txt_put;
}


/*	Zeichen ausgeben
*/

static void savechar(int c, PGFMT *filter)
{
	filter->save += io_nputc(c, filter->tmp, 1);
	filter->cs_current++;
}


static int txt_put(int c, PGFMT *io)
{
	if	(c == '\n')
	{
		if	(io->save)
		{
			savechar(' ', io);
			pgf_save(io);
		}

		if	(io->TP_flag)
		{
			pgf_print(io, 0);
			io->TP_flag = 0;
		}
		else if	(io->nofill || io->nl_flag)
		{
			pgf_print(io, 1);

			if	(io->nl_flag == 1)
			{
				pgf_newline(io);
				io->nl_flag = 2;
				return 0;
			}
		}

		io->nl_flag = 1;
		return 0;
	}

	if	(max(io->xpos, io->indent) + io->cs_save
			+ io->cs_current >= io->wpmargin)
	{
		pgf_print(io, 1);
	}

	if	(c == '.' && io->nl_flag)
	{
		if	(io->save)
		{
			savechar(' ', io);
			pgf_save(io);
		}

		io->put = arg_put;
		return 0;
	}

	io->nl_flag = 0;

	if	(c == '\\')
	{
		io->put = esc_put;
		return 0;
	}

	if	(c == '\t')
	{
		savechar(' ', io);
		pgf_save(io);
		pgf_print(io, 0);

		if	(io->tabsize != 0)
		{
			io->indent = io->xpos - io->leftmargin;
			io->indent += io->tabsize - 1;
			io->indent /= io->tabsize;
			io->indent *= io->tabsize;
			io->indent += io->leftmargin;
		}

		return 1;
	}

	savechar(c, io);

	if	(c == ' ' || c == '-')
	{
		pgf_save(io);
	}

	return 1;
}


static int ignore_put(int c, PGFMT *io)
{
	if	(c == '\n')
	{
		io->put = txt_put;
		io->nl_flag = 1;
	}

	return 0;
}


static int arg_put(int c, PGFMT *io)
{
	if	(c == '\n')
	{
		char *p;

		io_putc(0, io->tmp);
		io_rewind(io->tmp);
		p = io_mread(io->tmp, io->save);
		io_rewind(io->tmp);
		io->save = 0;
		io->cs_current = 0;
		cmd_eval(io, p);
		FREE(p);

		io->put = txt_put;
		io->save_put = NULL;
		io->nl_flag = 1;
	}
	else if	(c == '\\')
	{
		io->put = xesc_put;
		io->save_put = arg_put;
	}
	else	io->save += io_nputc(c, io->tmp, 1);

	return 0;
}


static int esc_put(int c, PGFMT *filter)
{
	switch (c)
	{
	case '%':	pgf_save(filter); filter->hyphen = "-"; return 0;
	case 'f':	filter->put = att_put; return 0;
	case 'c':	filter->put = ignore_put; return 0;
	case 'e':	savechar('\\', filter); break;
	case '&':	break;
	default:	savechar(c, filter); break;
	}

	restore_put(filter);
	return 0;
}


static int xesc_put(int c, PGFMT *filter)
{
	switch (c)
	{
	case '"':	c = 0; break;
	case 'f':	filter->put = att_put; return 0;
	default:	filter->save += io_nputc('\\', filter->tmp, 1); break;
	}

	filter->save += io_nputc(c, filter->tmp, 1);
	restore_put(filter);
	return 0;
}

static int att_put(int c, PGFMT *filter)
{
	if	(filter->save_put == arg_put)
	{
		filter->save += io_puts("\\f", filter->tmp);
		filter->save += io_nputc(c, filter->tmp, 1);
	}
	else
	{
		switch (c)
		{
		case 'R':	saveatt("R", filter); break;
		case 'I':	saveatt("I", filter); break;
		case 'B':	saveatt("B", filter); break;
		default:	break;
		}
	}

	restore_put(filter);
	return 0;
}


/*	Aufräumen
*/

/*	Kontrollfunktion
*/

static int pgf_ctrl(PGFMT *pgf, int req, va_list list)
{
	int stat;

	switch (req)
	{
	case IO_CLOSE:

		pgf_save(pgf);
		pgf_print(pgf, 1);
		/*
		pgf_put('\n', pgf);
		*/

		io_close(pgf->tmp);
		stat = io_close(pgf->io);
		memfree(pgf);
		return stat;

	default:

		if	(pgf->io->ctrl)
		{
			return pgf->io->ctrl(pgf->io->data, req, list);
		}
		else	return EOF;
	}
}


static void print_word(io_t *io, stack_t **ptr)
{
	if	(*ptr != NULL)
	{
		char *p = popstack(ptr, NULL);
		print_word(io, ptr);
		do_puts(p, io);
		memfree(p);
	}
}

static void pgf_print(PGFMT *io, int flag)
{
	if	(io->xpos > io->indent)
		pgf_newline(io);

	if	(io->word)
	{
		io->xpos += io_nputc(' ', io->io, io->indent - io->xpos);
		print_word(io->io, &io->word);

		if	(io->hyphen)
			io->xpos += io_puts(io->hyphen, io->io);

		io->xpos += io->cs_save;
		io->cs_save = 0;
	}

	if	(io->xpos && flag)
		pgf_newline(io);

	io->indent = io->save_indent;
}


static void pgf_save(PGFMT *pgf)
{
	if	(pgf->save != 0)
	{
		io_rewind(pgf->tmp);
		pushstack(&pgf->word, io_mread(pgf->tmp, pgf->save));
		io_rewind(pgf->tmp);
		pgf->cs_save += pgf->cs_current;
	}

	pgf->cs_current = 0;
	pgf->save = 0;
	pgf->hyphen = NULL;
}


static void pgf_newline(PGFMT *pgf)
{
	io_putc('\n', pgf->io);
	pgf->ypos++;
	pgf->xpos = 0;
}

static void cmd_eval(PGFMT *io, const char *p)
{
	int i;
	char *arg;

	while (isspace(*p))
		p++;

	for (i = 0; i < tabsize(cmd_def); i++)
	{
		if	(patcmp(cmd_def[i].name, p, &arg))
		{
			while (arg && isspace(*arg))
				arg++;

			cmd_def[i].eval(io, arg, cmd_def[i].par);
			return;
		}
	}
}


static void set_indent(PGFMT *pgf)
{
	if	(pgf->ind_stack)
	{
		pgf->indent = (int) (size_t) pgf->ind_stack->data;
	}
	else	pgf->indent = pgf->leftmargin;

	pgf->save_indent = pgf->indent;
}


static char *save_str(const char *str, PGFMT *filter, int flag)
{
	int mode;
	int n;

	for (mode = 0; *str != 0; str++)
	{
		if	(*str == '"')
		{
			mode = !mode;
			continue;
		}
		else if	(flag && !mode && isspace(*str))
		{
			do	str++;
			while (isspace(*str));

			break;
		}
		else if	(*str == '\\')
		{
			switch (str[1])
			{
			case 'f':

				switch (str[2])
				{
				case 'I':	saveatt("I", filter); break;
				case 'B':	saveatt("B", filter); break;
				default:	saveatt("R", filter); break;
				}

				str += 2;
				continue;

			case '"':
			case '\\':

				str++; break;

			default:

				break;
			}
		}

		n = io_nputc(*str, filter->tmp, 1);
		filter->save += n;
		filter->cs_current += n;
	}

	return (char *) str;
}


static void saveatt(const char *att, PGFMT *pgf)
{
	switch (*att)
	{
	case 'B':	pgf->save += io_nputc(CTRL_BOLD, pgf->tmp, 1); break;
	case 'I':	pgf->save += io_nputc(CTRL_ITALIC, pgf->tmp, 1); break;
	case 'R':	pgf->save += io_nputc(CTRL_ROMAN, pgf->tmp, 1); break;
	default:	break;
	}
}


static void c_SH(PGFMT *filter, const char *arg, int par)
{
	pgf_print(filter, 1);
	pgf_newline(filter);

	while (filter->ind_stack)
		popstack(&filter->ind_stack, NULL);

	saveatt("B", filter);
	save_str(arg, filter, 0);
	saveatt("R", filter);
	pgf_save(filter);
	filter->indent = par;
	pgf_print(filter, 1);
	set_indent(filter);
	pgf_newline(filter);
}


static void c_PP(PGFMT *filter, const char *arg, int par)
{
	pgf_print(filter, 1);
	pgf_newline(filter);
	set_indent(filter);
}


static void c_RS(PGFMT *filter, const char *arg, int par)
{
	filter->indent += std_val(arg, filter->tabsize);
	pushstack(&filter->ind_stack, (void *) (size_t) filter->indent);
}

static void c_RE(PGFMT *filter, const char *arg, int par)
{
	int n;

	n = std_val(arg, 1);

	while (n-- >= 0 && filter->ind_stack)
		popstack(&filter->ind_stack, NULL);

	set_indent(filter);
}


static void c_HP(PGFMT *filter, const char *arg, int par)
{
	pgf_print(filter, 1);
	pgf_newline(filter);
	set_indent(filter);
	filter->save_indent += std_val(arg, filter->tabsize);
}


static void c_TP(PGFMT *filter, const char *arg, int par)
{
	pgf_print(filter, 1);
	set_indent(filter);
	filter->save_indent += std_val(arg, filter->tabsize);
	filter->TP_flag = 1;
}

static void c_IP(PGFMT *filter, const char *arg, int par)
{
	char *p;

	pgf_print(filter, 1);
	p = save_str(arg, filter, 1);
	set_indent(filter);
	pgf_save(filter);
	pgf_print(filter, 0);
	filter->save_indent += std_val(p, filter->tabsize);
	filter->indent = filter->save_indent;
}


static void c_RBI2(PGFMT *filter, const char *arg, int par)
{
	char *att[2];
	int i;

	switch (par >> 8)
	{
	case 'I':	att[0] = "I"; break;
	case 'B':	att[0] = "B"; break;
	default:	att[0] = "R"; break;
	}

	switch (par & 0xFF)
	{
	case 'I':	att[1] = "I"; break;
	case 'B':	att[1] = "B"; break;
	default:	att[1] = "R"; break;
	}

	for (i = 0; *arg != 0; i++)
	{
		saveatt(att[i % 2], filter);
		arg = save_str(arg, filter, 1);
	}

	saveatt("R", filter);
	savechar(' ', filter);
	pgf_save(filter);

	if	(filter->TP_flag || filter->nofill)
	{
		pgf_print(filter, 0);
		filter->TP_flag = 0;
	}
}

static void c_RBI(PGFMT *filter, const char *arg, int par)
{
	char *p;

	switch (par)
	{
	case 'I':	p = "I"; break;
	case 'B':	p = "B"; break;
	default:	p = "R"; break;
	}

	saveatt(p, filter);
	save_str(arg, filter, 0);
	saveatt("R", filter);
	savechar(' ', filter);
	pgf_save(filter);

	if	(filter->TP_flag || filter->nofill)
	{
		pgf_print(filter, 0);
		filter->TP_flag = 0;
	}
}

static void c_nf(PGFMT *filter, const char *arg, int par)
{
	filter->nofill = par;
}

static void c_br(PGFMT *filter, const char *arg, int par)
{
	pgf_print(filter, 1);
	filter->TP_flag = 0;
}

static void c_nr(PGFMT *filter, const char *arg, int par)
{
	char *p;

	if	(patcmp("IN", arg, &p))
	{
		filter->leftmargin = atoi(p);
	}
}


static int std_val(const char *arg, int val)
{
	int n;

	n = atoi(arg);
	return (n != 0 ? n : val);
}


static void do_puts (char *str, io_t *io)
{
	if	(str == NULL)	return;

	for (; *str != 0; str++)
	{
		switch (*str)
		{
		case CTRL_ROMAN:	io_ctrl(io, IO_ROMAN_FONT); break;
		case CTRL_BOLD:		io_ctrl(io, IO_BOLD_FONT); break;
		case CTRL_ITALIC:	io_ctrl(io, IO_ITALIC_FONT); break;
		default:		io_putc(*str, io); break;
		}
	}
}
