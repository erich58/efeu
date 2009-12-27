/*	Auswertungsfunktion für Sourcefiles
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#include "src2doc.h"
#include <EFEU/strbuf.h>
#include <EFEU/efio.h>

static void subcopy (io_t *ein, io_t *aus, int c);
static void copy_str (io_t *ein, io_t *aus, int delim);


static void copy_str (io_t *ein, io_t *aus, int delim)
{
	int c, flag;

	flag = 0;
	io_protect(ein, 1);

	while ((c = io_getc(ein)) != EOF)
	{
		io_putc(c, aus);

		if	(flag)		flag = 0;
		else if	(c == delim)	break;
		else if	(c == '\\')	flag = 1;
	}

	io_protect(ein, 0);
}

static void subcopy (io_t *ein, io_t *aus, int c)
{
	io_putc(c, aus);

	switch (c)
	{
	case '"':
	case '\'':	copy_str(ein, aus, c); break;
	case '{':	copy_block(ein, aus, '}'); break;
	case '(':	copy_block(ein, aus, ')'); break;
	case '[':	copy_block(ein, aus, ']'); break;
	default:	break;
	}
}

void copy_block (io_t *ein, io_t *aus, int end)
{
	char *prompt;
	int c;

	prompt = io_prompt(ein, ">>> ");

	while ((c = io_skipcom(ein, NULL, 0)) != EOF && c != end)
		subcopy(ein, aus, c);

	io_putc(end, aus);
	io_prompt(ein, prompt);
}


static void skipline (SrcData_t *data, int c)
{
	SrcData_copy(data, NULL);

	do	subcopy(data->ein, NULL, c);
	while ((c = io_skipcom(data->ein, NULL, 0)) != EOF && c != '\n');
}

/*	Präprozessorzeile umkopieren
*/

void ppcopy (io_t *ein, strbuf_t *val, strbuf_t *com)
{
	int c;

	while ((c = io_skipcom(ein, com, 0)) != EOF)
	{
		if	(c == '\n')	break;
		if	(c == '\\')	c = io_getc(ein);

		if	(val)	sb_putc(c, val);
	}
}



static void ppline (SrcData_t *data)
{
	char *name;
	int n;

	name = parse_name(data->ein, skip_blank(data->ein));

	for (n = 0; n < data->ppdim; n++)
	{
		if	(mstrcmp(name, data->ppdef[n].name) == 0)
		{
			data->ppdef[n].eval(data, data->ppdef[n].name);
			return;
		}
	}

	SrcData_copy(data, NULL);
	ppcopy(data->ein, NULL, NULL);
}

static void keyline (SrcData_t *data, int c)
{
	Decl_t *decl;
	io_t *aus;
	decl = parse_decl(data->ein, c);

	if	(decl->type & data->mask)
	{
		reg_set(1, mstrncpy(decl->def + decl->start,
			decl->end - decl->start));
		aus = io_strbuf(data->synopsis);
		Decl_print(decl, aus);
		io_close(aus);
		sb_printf(data->tab[BUF_DESC], "\n/* %s */\n", reg_get(1));
		SrcData_copy(data, data->tab[BUF_DESC]);
	}
	else	SrcData_copy(data, NULL);

	while ((c = io_skipcom(data->ein, NULL, 0)) != EOF && c != '\n')
		subcopy(data->ein, NULL, c);
}


void SrcData_eval (SrcData_t *data)
{
	int c;

	io_ungetc(skip_space(data->ein, data->buf), data->ein);

	if	(sb_getpos(data->buf))
	{
		char *title, *p;

		sb_putc(0, data->buf);
		title = (char *) data->buf->data;
		p = strchr(title, '\n');
		memfree(data->var[VAR_TITLE]);
		data->var[VAR_TITLE] = p ? mstrncpy(title, p - title) :
			mstrcpy(title);
		SrcData_copy(data, NULL);
	}

	while ((c = skip_space(data->ein, data->buf)) != EOF)
	{
		if	(c == '#')
		{
			ppline(data);
		}
		else if	(isalpha(c) || c == '_')
		{
			keyline(data, c);
		}
		else	skipline(data, c);
	}

	SrcData_copy(data, NULL);
}
