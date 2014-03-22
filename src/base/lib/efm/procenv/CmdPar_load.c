/*
:*:load command configuration from file
:de:Kommandoparameter aus Datei laden

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/CmdPar.h>
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/appl.h>
#include <ctype.h>

static void skipline (IO *io)
{
	int c;

	do	c = io_getc(io);
	while	(c != EOF && c != '\n');
}

static void addline (IO *io, StrBuf *buf, int end)
{
	int c;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')	break;

		sb_putc(c, buf);
	}

	sb_putc(end, buf);
}

static void buf2var(CmdPar *par, const char *name, StrBuf *buf)
{
	IO *io;
	CmdParVar *var;
	StrBuf *vbuf;
	int c;

	vbuf = sb_acquire();
	sb_setpos(buf, 0);
	io = langfilter(io_strbuf(buf), NULL);

	while ((c = io_getucs(io)) != EOF)
		sb_putucs(c, vbuf);

	io_close(io);

	var = CmdPar_var(par, name, 1);
	memfree(var->value);
	var->value = sb_cpyrelease(vbuf);
	sb_trunc(buf);
}

static void langsub (StrBuf *sb)
{
	IO *io;
	int c;

	io = langfilter(io_mstr(sb_strcpy(sb)), NULL);
	sb_begin(sb);

	while ((c = io_getc(io)) != EOF)
		sb_putc(c, sb);

	io_close(io);
}

/*	Beschreibungstext kopieren
*/

static char *copy_desc (IO *io, StrBuf *sb)
{
	int c, flag;

	sb_begin(sb);
	flag = 0;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '#')
		{
			do	c = io_getc(io);
			while	(c != EOF && c != '\n');

			continue;
		}

		if	(c != '\t')
		{
			io_ungetc(c, io);
			break;
		}

		io_protect(io, 1);

		while ((c = io_getc(io)) != EOF)
		{
			if	(c == '\\')
			{
				int c2 = io_getc(io);

				if	(c2 == '\n')	break;

				switch (c2)
				{
				case 'n':	sb_putc('\n', sb); continue;
				case 't':	c = '\t'; break;
				case '0':	c = 0; break;
				case EOF:	c = '\\'; break;
				default:	io_ungetc(c2, io); break;
				}
			}

			sb_putc(c, sb);

			if	(c == '\n')	break;
			if	(c == ':')	flag = 1;
		}

		io_protect(io, 0);
	}

	if	(flag)
		langsub(sb);

	return sb_strcpy(sb);
}

/*	Regulären Ausdruck kopieren
*/

static char *copy_expr (IO *io, StrBuf *sb)
{
	int c;

	sb_begin(sb);
	io_protect(io, 1);

	while ((c = io_xgetc(io, "/")) != EOF)
		sb_putc(c, sb);

	io_protect(io, 0);
	io_getc(io);
	return sb_strcpy(sb);
}


/*	String kopieren
*/

static void copy_str (IO *io, StrBuf *sb, int key)
{
	char delim[2];
	int c;

	io_protect(io, 1);
	delim[0] = key;
	delim[1] = 0;

	while ((c = io_xgetc(io, delim)) != EOF)
		sb_putc(c, sb);

	io_getc(io);
	io_protect(io, 0);
}


static void set_desc (CmdParVar *var, char *desc)
{
	if	(var && desc)
	{
		memfree(var->desc);
		var->desc = desc;
	}
	else	memfree(desc);
}


static int get_argtype (IO *io)
{
	if	(io_peek(io) == ':')
	{
		io_getc(io);
		return ARGTYPE_OPT;
	}
	else	return ARGTYPE_STD;
}


static CmdParKey *create_key (CmdParKey *buf, StrBuf *sb)
{
	CmdParKey *key;

	if	(!buf->key && buf->partype != PARTYPE_ARG)
	{
		buf->key = sb_strcpy(sb);
		sb_begin(sb);
	}

	if	(!buf->partype)
	{
		if	(!buf->key || !buf->key[0])
		{
			memfree(buf->key);
			memfree(buf->val);
			return NULL;
		}

		buf->partype = PARTYPE_OPT;
	}

	key = CmdParKey_alloc();
	*key = *buf;
	key->arg = sb_strcpy(sb);
	return key;
}

/*	Zuweisungswert bestimmen
*/

static char *getval (StrBuf *sb, int flag)
{
	char *p;

	if	(sb->pos == 0)
		return NULL;

	if	(sb->data[0] == ':')
	{
		langsub(sb);
	}
	else if	(sb->data[0] == '$' && sb->pos > 1 
		&& sb->data[sb->pos - 1] == '$')
	{
		int i;

		do	sb->pos--;
		while	(isspace(sb->data[sb->pos - 1]));

		sb->pos--;

		for (i = 0; i < sb->pos; i++)
			sb->data[i] = sb->data[i + 1];

		sb_sync(sb);
	}

	p = sb_strcpy(sb);

	if	(flag && mstrcmp(p, "NULL") == 0)
	{
		memfree(p);
		p = NULL;
	}

	return p;
}

static int f_assign (CmdPar *cpar, CmdParVar *var,
	const char *par, const char *arg)
{
	memfree(var->value);
	var->value = mstrcpy(par);
	return 0;
}

static CmdParEval eval_assign = { NULL, NULL, f_assign };

static void key2call (CmdParCall *call, CmdParKey *key,
	IO *io, StrBuf *sb, int flag)
{
	memset(call, 0, sizeof(CmdParCall));
	memfree(key->val);
	call->name = key->key;
	call->eval = NULL;

	if	(key->argtype == 0)
	{
		call->name = sb_strcpy(sb);
	}
	else if	(key->argtype == ARGTYPE_VALUE)
	{
		call->par = getval(sb, flag);
		call->eval = &eval_assign;
	}
	else if	(sb_getpos(sb))
	{
		char *p;

		sb_putc(0, sb);

		for (p = (char *) sb->data; *p != 0; p++)
		{
			if	(*p == ' ')
			{
				*p = 0;
				call->par = mstrcpy(p + 1);
				break;
			}
		}

		call->eval = CmdParEval_get((char *) sb->data);

		if	(!call->eval)
			io_error(io, "[efm:21]", "s", sb->data);
	}
}


static void eval_call (CmdPar *par, CmdParCall *call)
{
	if	(call->eval || call->par)
	{
		CmdParCall_eval(par, call, NULL);
	}
	else	CmdPar_var(par, call->name, 1);

	memfree(call->name);
	memfree(call->par);
}


static void parse_line (CmdPar *par, IO *io, StrBuf *sb)
{
	CmdParKey *key, **kp;
	CmdParCall *call, **cp;
	int c;

	key = NULL;
	kp = &key;
	call = NULL;
	cp = &call;

	while ((c = io_getc(io)) != EOF)
	{
		CmdParKey kbuf;
		CmdParCall cbuf;
		CmdParDef *def;
		CmdParVar *var;
		int needspace;
		int ignorespace;
		int checknull;

		sb_begin(sb);
		memset(&kbuf, 0, sizeof(CmdParKey));
		needspace = 0;
		ignorespace = 1;
		checknull = 0;

	/*	Test des ersten Zeichens
	*/
		switch (c)
		{
		case '@':
			kbuf.partype = PARTYPE_ENV;
			break;
		case ':':
			kbuf.partype = PARTYPE_ARG;
			kbuf.argtype = get_argtype(io);
			break;
		case '$':
			kbuf.partype = PARTYPE_ARG;
			kbuf.argtype = ARGTYPE_LAST;
			break;
		case '*':
			kbuf.partype = PARTYPE_ARG;
			kbuf.argtype = ARGTYPE_VA0;
			break;
		case '+':
			kbuf.partype = PARTYPE_ARG;
			kbuf.argtype = ARGTYPE_VA1;
			break;
		case '/':
			kbuf.partype = PARTYPE_ARG;
			kbuf.argtype = ARGTYPE_REGEX;
			kbuf.key = copy_expr(io, sb);
			sb_begin(sb);
			break;
		default:
			io_ungetc(c, io);
			break;
		}

	/*	Zeichen kopieren und auf Abschluß testen
	*/
		while ((c = io_getc(io)) != '|')
		{
			switch (c)
			{
			case ' ':
			case '\t':
				if	(ignorespace)	continue;

				needspace = 1;
				ignorespace = 1;
				continue;
			case ':':
				if	(kbuf.argtype || kbuf.partype)
					break;

				kbuf.argtype = get_argtype(io);
				kbuf.key = sb_strcpy(sb);
				sb_begin(sb);
				needspace = 0;
				ignorespace = 1;
				continue;
			case '[':
				memfree(kbuf.val);
				kbuf.val = io_xgets(io, "[]");
				io_getc(io);
				needspace = 0;
				ignorespace = 1;
				continue;
			case '=':
				if	(kbuf.argtype || kbuf.partype)
					break;

				if	(sb_getpos(sb) == 0)
					break;

				kbuf.argtype = ARGTYPE_VALUE;
				kbuf.key = sb_strcpy(sb);
				sb_begin(sb);
				needspace = 0;
				ignorespace = 1;
				checknull = 1;
				continue;
			case ';':
				key2call(&cbuf, &kbuf, io, sb, checknull);
				var = CmdPar_var(par, cbuf.name, 1);

				if	(key)
				{
					*cp = CmdParCall_alloc();
					**cp = cbuf;
					cp = &(*cp)->next;
				}
				else	eval_call(par, &cbuf);

				sb_begin(sb);
				memset(&kbuf, 0, sizeof(CmdParKey));
				needspace = 0;
				ignorespace = 1;
				continue;
			case EOF:
			case '\n':
				key2call(&cbuf, &kbuf, io, sb, checknull);
				var = CmdPar_var(par, cbuf.name, 1);

				if	(key)
				{
					*cp = CmdParCall_alloc();
					**cp = cbuf;
					cp = &(*cp)->next;

					def = CmdParDef_alloc();
					def->key = key;
					def->call = call;
					def->desc = copy_desc(io, sb);
					CmdPar_add(par, def);
				}
				else
				{
					set_desc(var, copy_desc(io, sb));
					eval_call(par, &cbuf);
				}

				return;
			case '\'':
			case '"':
				if	(needspace)
					sb_putc(' ', sb);

				copy_str(io, sb, c);
				checknull = 0;
				needspace = 0;
				ignorespace = 0;
				continue;
			case '\\':
				c = io_getc(io);

				switch (c)
				{
				case '\n':	continue;
				case EOF:	c = '\\'; break;
				case 'n':	c = '\n'; break;
				case 't':	c = '\t'; break;
				case '0':	c = 0; break;
				default:	break;
				}

				checknull = 0;
				break;
			default:
				break;
			}

			if	(needspace)
				sb_putc(' ', sb);

			sb_putc(c, sb);
			needspace = 0;
			ignorespace = 0;
		}

		if	((*kp = create_key(&kbuf, sb)) != NULL)
			kp = &(*kp)->next;
	}
}


/*
:*:The function |$1| loads command configurations from IO-structure <io>.
If <flag> is set, ident and copyright informations are extracted
from comment head.
:de:Die Funktion |$1| liest Kommandoparameter aus der IO-Struktur <io>.
Falls <flag> gesetzt ist, wird Bezeichnung und Kopierrechtsinformat
aus dem Kommentarkopf extrahiert.
*/

void CmdPar_read (CmdPar *par, IO *io, int end, int flag)
{
	StrBuf buf_data;
	StrBuf *sb;
	StrBuf *cbuf;
	int32_t c;

	if	(io == NULL)	return;

	par = CmdPar_ptr(par);
	sb_init(&buf_data, 0);
	sb = &buf_data;
	io = io_lnum(io_refer(io));
	cbuf = flag ? sb : NULL;

	while ((c = io_getc(io)) != EOF && c != end)
	{
		if	(flag && c == '#')
		{
			do	c = io_getc(io);
			while	(c == ' ' || c == '\t');

			if	(cbuf)
			{
				if	(c == '\n')
				{
					buf2var(par, "Ident", cbuf);
					cbuf = NULL;
				}
				else
				{
					sb_putc(c, cbuf);
					addline(io, cbuf, '\n');
				}
			}
			else if	(c == '$')
			{
				addline(io, sb, 0);

				switch (sb->data[0])
				{
				case 'C':
				case 'c':
					buf2var(par, "Copyright", sb);
					break;
				}
				
				sb_clean(sb);
			}
			else	skipline(io);

			continue;
		}

		if	(cbuf)
		{
			buf2var(par, "Ident", cbuf);
			cbuf = NULL;
		}

		switch (c)
		{
		case '\n':
			break;
		case '\t':
		case '=':
		case '#':
			skipline(io);
			break;
		default:
			io_ungetc(c, io);
			parse_line(par, io, sb);
			flag = 0;
			break;
		}
	}

	sb_free(&buf_data);
	io_close(io);
}


/*
:*:The function |$1| loads command configurations for
the name <name>. If <flag> is set, ident and copyright informations
are extracted from comment head.
:de:Die Funktion |$1| ladet Kommandoparameter zu dem Kommandonamen <name>.
Falls <flag> gesetzt ist, wird Bezeichnung und Kopierrechtsinformation
aus dem Kommentarkopf extrahiert.
*/

int CmdPar_load (CmdPar *par, const char *name, int flag)
{
	IO *io = io_applfile(name, APPL_CNF);

	if	(io)
	{
		CmdPar_read (par, io, EOF, flag);
		io_close(io);
		return 1;
	}

	return 0;
}

/*
$SeeAlso
\mref{CmdPar(3)},
\mref{CmdParCall(3)},
\mref{CmdParDef(3)},
\mref{CmdParEval(3)},
\mref{CmdParExpand(3)},
\mref{CmdParKey(3)},
\mref{CmdParVar(3)},
\mref{CmdPar_eval(3)},
\mref{CmdPar_list(3)},
\mref{CmdPar_psub(3)},
\mref{CmdPar_usage(3)},
\mref{CmdPar_write(3)},
\mref{Resource(3)},
\mref{CmdPar(7)}.
*/

