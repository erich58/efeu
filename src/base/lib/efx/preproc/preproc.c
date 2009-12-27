/*
Eingabe - Preprozessor

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/ftools.h>
#include <EFEU/efmain.h>
#include <EFEU/KeyTab.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/preproc.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

#define	NEED_PROTECT	0	/* Schutzmode vorläufig nicht benötigt */

/*	Teststruktur: Die Variable depth gibt die Verschachtelungstiefe
	in einem nicht selektierten Zweig an.
*/

#define	PPS_PRE		0	/* Vor Selektion */
#define	PPS_POST	1	/* Nach Selektion */
#define	PPS_END		2	/* Ende der Selektion (nach PP_ELSE) */


typedef struct pptest_s {
	struct pptest_s *next;	/* Verweis auch nächsten Eintrag */
	int depth;		/* Falsch - Verschachtelungstiefe */
	int stat;		/* Status */
	int start;		/* Startzeile */
} pptest_t;


/*	Eingabekette
*/

typedef struct ppinput_s {
	struct ppinput_s *next;	/* Verweis auch nächsten Eintrag */
	io_t *io;		/* Eingabestruktur */
	pptest_t *test;		/* Testliste */
	int crflag;		/* Flag für '\r' in Eingabefile */
} ppinput_t;


/*	Filterstruktur
*/

typedef struct {
	char *ident;		/* Buffer mit Identifikation */
	char **pptr;		/* Pointer auf Include-Pfad */
	ppinput_t *input;	/* Eingabeliste */
	strbuf_t *buf;		/* Zwischenbuffer */
	strbuf_t *combuf;	/* Kommentarbuffer */
	int at_start;		/* Flag für Zeilenbeginn */
	int save;		/* Zahl der gebufferten Zeichen */
#if	NEED_PROTECT
	int protect;		/* Abbruchschutz */
#endif
	int expand;		/* Flag für String und Makrointerpretation */
} pp_t;


/*	Hilfsfunktionen
*/

static int pp_get (pp_t *ptr);
static int pp_ctrl (pp_t *ptr, int req, va_list list);

static void pptest_start (ppinput_t *input, int flag);
static void pptest_end (ppinput_t *input);

static ppinput_t *new_input (io_t *io);
static int subclose (pp_t *pp);

static ALLOCTAB(pptest_tab, 0, sizeof(pptest_t));

static void pp_cmd (pp_t *pp);
static void pp_subcmd (pp_t *pp, int code);

static MacDef_t pp_macdef[] = {
	{ "defined(x)", macsub_def },
};

static xtab_t *pp_macrotab = NULL;


/*	Preprozess - Filter: Anhand von IOPP_COMMENT wird geprüft,
	ob er bereits vorgeschalten wurde.
*/

io_t *io_ptrpreproc (io_t *io, char **pptr)
{
	if	(io && io_ctrl(io, IOPP_COMMENT, NULL) == EOF)
	{
		pp_t *pp = memalloc(sizeof(pp_t));
		pp->pptr = pptr;
		pp->ident = NULL;
		pp->input = new_input(io);
		pp->buf = new_strbuf(0);
		pp->combuf = new_strbuf(0);
		pp->at_start = 1;
		pp->expand = 1;
		pp->save = 0;
#if	NEED_PROTECT
		pp->protect = 0;
#endif
		io = io_alloc();
		io->get = (io_get_t) pp_get;
		io->ctrl = (io_ctrl_t) pp_ctrl;
		io->data = pp;
	}

	return io;
}


/*	Die Funktion pp_eat liefert das nächste Zeichen.
	Eventell vorhandene Zeilenfortsetzungen und Kommentare
	werden entfernt. Falls flag gesetzt ist, wird ein
	nach einem Kommentar folgender Zeilenvorschub entfernt.
*/

static int pp_eat (ppinput_t *in, strbuf_t *buf, int flag)
{
	int c;

	while ((c = io_skipcom(in->io, buf, flag)) == '\\')
	{
		if	(io_peek(in->io) != '\n')
			break;

		io_getc(in->io);
		io_linemark(in->io);

		do	c = io_getc(in->io);
		while	(c == ' ' || c == '\t');
		
		io_ungetc(c, in->io);
	}

	if	(c == '\r')
	{
		if	(!in->crflag)
			io_message(in->io, PREPROC, 199, 0);

		in->crflag = 1;
		c = io_getc(in->io);
	}

	return c;
}


static void pp_skip (io_t *io, int flag)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		switch (c)
		{
		case '\\':

			io_getc(io);
			break;

		case '\n':

			if	(flag)
				io_ungetc(c, io);
			return;
		}
	}
}

/*	Zeichen lesen
*/

static int pp_get(pp_t *pp)
{
	ppinput_t *x;
	io_t *tmp;
	int flag;
	int c;

	while (pp->save == 0)
	{
		x = pp->input;

		if	(x == NULL)	return EOF;

		c = pp_eat(x, pp->combuf, pp->at_start);

		if	(c == EOF)
		{
			if	(x->test)
			{
				char *p = msprintf("%d", x->test->start);
				io_error(x->io, PREPROC, 196, 1, p);
				memfree(p);
			}

			subclose(pp);
			continue;
		}

		flag = (x->test && x->test->depth);

	/*	Zeilenvorschub
	*/
		if	(c == '\n')
		{
			pp->at_start = 1;
			
			if	(flag)	continue;

			return c;
		}

		sb_begin(pp->buf);

		if	(pp->at_start)
		{
			while (c == ' ' || c == '\t')
			{
				sb_putc(c, pp->buf);
				c = pp_eat(x, pp->combuf, 0);
			}

			if	(c == '#')
			{
				c = pp_eat(x, pp->combuf, 0);
				
				if	(c == '#')
				{
					pp->at_start = 0;

					if	(flag)	continue;

					sb_putc(c, pp->buf);
					pp->save = sb_getpos(pp->buf);
					sb_begin(pp->buf);
					break;
				}

				while (c == ' ' || c == '\t')
					c = pp_eat(x, pp->combuf, 0);

				if	(isalpha(c) || c == '_')
				{
					io_ungetc(c, x->io);
					pp_cmd(pp);
					continue;
				}

				pp->save = flag ? 0 : sb_getpos(pp->buf);
				pp_skip(x->io, pp->save);
				sb_begin(pp->buf);
				continue;
			}
		}

		if	(flag)
		{
			iocpy_skip(x->io, NULL, 0, "\n", 1);
			continue;
		}

		pp->save = sb_getpos(pp->buf);
		pp->at_start = 0;
		
		if	(pp->save)
		{
			io_ungetc(c, x->io);
			sb_begin(pp->buf);
			break;
		}

		if	(pp->expand)
		{
			tmp = io_strbuf(pp->buf);
			flag = 1;

			if	(isalpha(c) || c == '_')
			{
				iocpy_macsub(x->io, tmp, c, "!%n_", 1);
			}
			else if	(c =='"')
			{
				iocpy_str(x->io, tmp, c, "\"", 1);
			}
			else if	(c =='\'')
			{
				iocpy_str(x->io, tmp, c, "'", 1);
			}
			else	flag = 0;

			io_close(tmp);

			if	(flag)
			{
				pp->save = sb_getpos(pp->buf);
				sb_begin(pp->buf);
				break;
			}
		}
		
		return c;
	}

	if	(pp->save > 0)
	{
		pp->save--;
		return sb_getc(pp->buf);
	}

	return EOF;
}


/*	Neue Eingabestruktur generieren
*/

static ppinput_t *new_input(io_t *io)
{
	ppinput_t *x;

	x = ALLOC(1, ppinput_t);
	x->io = io_lnum(io);
	x->next = NULL;
	x->test = NULL;
	x->crflag = 0;
	return x;
}


/*	Aufraumen
*/

static int subclose(pp_t *pp)
{
	ppinput_t *x;
	pptest_t *t;
	int stat;

	x = pp->input;

	while (x->test != NULL)
	{
		t = x->test;
		x->test = t->next;
		del_data(&pptest_tab, t);
	}

	pp->input = x->next;
	stat = io_close(x->io);
	memfree(x);
	return stat;
}


/*	Kontrollfunktion
*/

static int pp_ctrl(pp_t *pp, int req, va_list list)
{
	int stat;
	char **p;

	switch (req)
	{
	case IO_CLOSE:

		stat = 0;

		while (pp->input != NULL)
			if (subclose(pp) != 0) stat = EOF;

		del_strbuf(pp->buf);
		del_strbuf(pp->combuf);
		memfree(pp->ident);
		memfree(pp);
		return stat;

#if	NEED_PROTECT

	case IO_PROTECT:

		stat = va_arg(list, int);

		if	(pp->input)
			io_protect(pp->input->io, stat);

		if	(stat)		pp->protect++;
		else if	(pp->protect)	pp->protect--;

		return pp->protect;
#endif

	case IOPP_EXPAND:

		stat = pp->expand;
		pp->expand = va_arg(list, int);
		return stat;

	case IOPP_COMMENT:

		if	((p = va_arg(list, char **)) != NULL)
			*p = sb_strcpy(pp->combuf);

		sb_clear(pp->combuf);
		return 0;

/*	Gesperrte Steuersequenzen
*/
	case IO_PEEK:
	case IO_REWIND:

		return EOF;
		
	default:

		if	(pp->input && pp->input->io->ctrl)
		{
			return (*pp->input->io->ctrl)(pp->input->io->data, req, list);
		}

		return EOF;
	}
}


/*	Präprozessbefehle
*/

#define	PP_INCL		1	/* Datei einbinden */
#define	PP_RETURN	2	/* Datei verlassen */
#define	PP_EXPAND	3	/* Makros expandieren */
#define	PP_NOEXPAND	4	/* Makros nicht expandieren */

#define	PP_DEFINE	11	/* Makro definieren */
#define	PP_UNDEF	12	/* Makro Löschen */
#define	PP_ERROR	13	/* Fehlermeldung ausgeben */

#define	PP_IF		21	/* Beginn Testblock */
#define	PP_IFDEF	22	/* Beginn Testblock */
#define	PP_IFNDEF	23	/* Beginn Testblock */
#define	PP_ELIF		24	/* Alternative */
#define	PP_ELSE		25	/* Standardwert */
#define	PP_ENDIF	26	/* Ende Testblock */


typedef struct {
	char *name;
	int code;
} pp_cmd_t;


/*	Standard Includenamen
*/

static pp_cmd_t pp_cdef[] = {
	{ "include", PP_INCL },
	{ "return", PP_RETURN },
	{ "expand", PP_EXPAND },
	{ "noexpand", PP_NOEXPAND },
	{ "define", PP_DEFINE },
	{ "undef", PP_UNDEF },
	{ "error", PP_ERROR },
	{ "if", PP_IF },
	{ "ifdef", PP_IFDEF },
	{ "ifndef", PP_IFNDEF },
	{ "elif", PP_ELIF },
	{ "else", PP_ELSE },
	{ "endif", PP_ENDIF },
};

static vecbuf_t *pp_tab = NULL;

static void cmd_include (pp_t *pp);
static void cmd_undef (io_t *io);
static int cmd_testval (io_t *io);
static int cmd_isdef (io_t *io);


/*	Direktiven ausführen
*/

static void pp_cmd(pp_t *pp)
{
	ppinput_t *x;
	pp_cmd_t *cmd;
	char *name;

	x = pp->input;
	name = io_mgets(x->io, "!%n_");

	if	(pp_tab == NULL)
	{
		pp_tab = KeyTab(16);
		StrKey_append(pp_tab, tabparm(pp_cdef));
	}

	if	((cmd = StrKey_get(pp_tab, name, NULL)) == NULL)
	{
		io_error(x->io, PREPROC, 191, 1, name);
		memfree(name);
		return;
	}

	memfree(name);
	io_eat(x->io, " \t");

	if	(x->test == NULL)
	{
		pp_subcmd(pp, cmd->code);
		return;
	}
	else if	(x->test->depth > 1)
	{
		switch (cmd->code)
		{
		case PP_ENDIF:	x->test->depth--; break;
		case PP_IF:
		case PP_IFDEF:
		case PP_IFNDEF:	x->test->depth++; break;
		default:	break;
		}
	}
	else if	(cmd->code == PP_ELIF)
	{
		if	(x->test->stat == PPS_POST)
		{
			iocpy_skip(x->io, NULL, 0, "\n", 0);
			x->test->depth = 1;
		}
		else if	(x->test->stat == PPS_END)
		{
			io_error(x->io, PREPROC, 193, 0);
		}
		else if	(cmd_testval(x->io))
		{
			x->test->stat = PPS_POST;
			x->test->depth = 0;
		}
		else	x->test->depth = 1;
	}
	else if	(cmd->code == PP_ELSE)
	{
		switch (x->test->stat)
		{
		case PPS_PRE:	x->test->depth = 0; break;
		case PPS_END:	io_error(x->io, PREPROC, 194, 0); break;
		default:	x->test->depth = 1; break;
		}

		x->test->stat = PPS_END;
	}
	else if	(cmd->code == PP_ENDIF)
	{
		pptest_end(x);
	}
	else if	(x->test->depth == 0)
	{
		pp_subcmd(pp, cmd->code);
		return;
	}
	else
	{
		switch (cmd->code)
		{
		case PP_IF:
		case PP_IFDEF:
		case PP_IFNDEF:	x->test->depth++; break;
		default:	break;
		}
	}

/*	Restliche Zeichen aus Zeile löschen
*/
	iocpy_skip(x->io, NULL, 0, "\n", 0);
}


/*	Preprozessorbefehle
*/

static void pp_subcmd(pp_t *pp, int code)
{
	io_t *io;

	io = pp->input->io;

	switch (code)
	{
	case PP_IF:	pptest_start(pp->input, cmd_testval(io)); break;
	case PP_IFDEF:	pptest_start(pp->input, cmd_isdef(io)); break;
	case PP_IFNDEF:	pptest_start(pp->input, !cmd_isdef(io)); break;
	case PP_ELIF:	io_error(io, PREPROC, 193, 0); break;
	case PP_ELSE:	io_error(io, PREPROC, 194, 0); break;
	case PP_ENDIF:	io_error(io, PREPROC, 195, 0); break;
	case PP_INCL:	cmd_include(pp); break;
	case PP_RETURN: subclose(pp); break;
	case PP_EXPAND: pp->expand = 1; break;
	case PP_NOEXPAND: pp->expand = 0; break;
	case PP_DEFINE:	AddMacro(ParseMacro(io)); break;
	case PP_UNDEF:	cmd_undef(io); break;
	case PP_ERROR:	iocpy_str(io, ioerr, 0, "\n", 1); return;
	default:	break;
	}

	iocpy_skip(io, NULL, 0, "\n", 0);
}


static void pptest_start(ppinput_t *input, int flag)
{
	pptest_t *t;

	t = new_data(&pptest_tab);
	t->start = io_ctrl(input->io, IO_LINE);
	t->next = input->test;
	input->test = t;

	if	(flag)
	{
		t->stat = PPS_POST;
		t->depth = 0;
	}
	else
	{
		t->stat = PPS_PRE;
		t->depth = 1;
	}
}


static void pptest_end(ppinput_t *input)
{
	pptest_t *t;

	t = input->test;
	input->test = t->next;
	del_data(&pptest_tab, t);
}

static io_t *open_include(const char *path, const char *name)
{
	char *fname;
	io_t *io;

	if	((fname = fsearch(path, NULL, name, NULL)) != NULL)
	{
		io = io_fileopen(fname, "r");
		AddDepend(fname);
		return io;
	}

	return NULL;
}

static void cmd_include(pp_t *pp)
{
	io_t *io;
	int flag;
	int c;
	io_t *tmp;
	char *name;

	tmp = io_tmpbuf(0);
	io = pp->input->io;
	io_macsub(io, tmp, "\n");
	io_rewind(tmp);

	c = io_getc(tmp);
	flag = 0;

	if	(c == '<')
	{
		name = io_mgets(tmp, ">");
		flag = 0;
	}
	else if	(c == '"')
	{
		name = io_mgets(tmp, "\"");
		flag = 1;
	}
	else	name = NULL;

	io_close(tmp);

	if	(name == NULL)
	{
		io_error(io, PREPROC, 198, 0);
		return;
	}

	io = flag ? open_include(NULL, name) : NULL;

	if	(io == NULL)
		io = open_include(pp->pptr ? *pp->pptr : NULL, name);

	if	(io != NULL)
	{
		ppinput_t *x;

		x = new_input(io);
		x->next = pp->input;
		pp->input = x;
	}
	else	io_error(pp->input->io, PREPROC, 197, 1, name);

	memfree(name);
}


static int cmd_testval(io_t *io)
{
	Obj_t *obj;
	int flag;
	io_t *tmp;

	tmp = io_tmpbuf(0);
	PushMacroTab(pp_macrotab);

	if	(pp_macrotab == NULL)
		AddMacDef(pp_macdef, tabsize(pp_macdef));

	io_macsub(io, tmp, "\n");
	pp_macrotab = PopMacroTab();

	io_putc('\n', tmp);
	io_rewind(tmp);
	obj = EvalObj(Parse_term(tmp, 0), &Type_bool);
	io_close(tmp);

	flag = (obj ? Val_bool(obj->data) : 0);
	UnrefObj(obj);
	return flag;
}


static void cmd_undef(io_t *io)
{
	Macro_t key;

	key.name = io_mgets(io, "%s");
	DelMacro(xsearch(MacroTab, &key, XS_DELETE));
	memfree(key.name);
}


static int cmd_isdef(io_t *io)
{
	Macro_t key;
	int flag;

	key.name = io_mgets(io, "%s");
	flag = (xsearch(MacroTab, &key, XS_FIND) != NULL);
	memfree(key.name);
	return flag;
}

io_t *io_cmdpreproc (io_t *io)
{
	return io_ptrpreproc(io, &IncPath);
}
