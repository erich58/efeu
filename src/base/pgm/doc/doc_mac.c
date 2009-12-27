/*	Dokumentbefehle ausführen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/parsedef.h>
#include "efeudoc.h"
#include <ctype.h>


/*	Argumentbuffer
*/

static VECBUF(arg_buf, 32, sizeof(char **));

#define nextarg	((char **) vb_next(&arg_buf))[0]


/*	Ausgabefunktionen
*/

typedef void (*pfunc_t) (strbuf_t *buf, const char *arg, int flag);

static void pf_std (strbuf_t *buf, const char *arg, int flag)
{
	if	(flag && arg)
		sb_putc(' ', buf);

	sb_puts(arg, buf);
}

static void pf_list (strbuf_t *buf, const char *arg, int flag)
{
	if	(flag)	sb_putc(',', buf);

	if	(!arg)	return;

	for (; *arg != 0; arg++)
	{
		if	(*arg == ',' || *arg == ')')
		{
			sb_putc('\'', buf);
			sb_putc(*arg, buf);
			sb_putc('\'', buf);
		}
		else	sb_putc(*arg, buf);
	}
}

static void pf_str (strbuf_t *buf, const char *arg, int flag)
{
	if	(flag)
		sb_puts("\", \"", buf);

	if	(arg)
	{
		io_t *out = io_strbuf(buf);
		io_xputs(arg, out, "\"");
		io_close(out);
	}
}

#define	pfunc(q,d)	((q) ? pf_str : ((d > 0) ? pf_list : pf_std))


/*	Argument substituieren
*/

static void arg_sub (strbuf_t *buf, io_t *io,
	pfunc_t put, int argc, char **argv)
{
	int c = io_getc(io);

	switch (c)
	{
	case EOF:

		return;

	case '#':

		sb_printf(buf, "%d", argc);
		break;

	case '*':

		for (c = 1; c < argc; c++)
			put(buf, argv[c], c > 1);

		break;

	default:

		if	(isdigit(c))
		{
			c -= '0';
			put(buf, (c < argc) ? argv[c] : NULL, 0);
		}
		else	sb_putc(c, buf);

		break;
	}
}

/*	Makro expandieren
*/

static char *mac_expand (const char *fmt, int argc, char **argv)
{
	strbuf_t *buf;
	io_t *in;
	int c, quote, depth;

	if	(fmt == NULL)	return NULL;

	buf = new_strbuf(0);
	in = io_cstr(fmt);
	quote = 0;
	depth = 0;

	while ((c = io_getc(in)) != EOF)
	{
		switch (c)
		{
		case '$':
			arg_sub(buf, in, pfunc(quote, depth), argc, argv);
			continue;
		case '\\':
			if	(io_peek(in) != EOF)
			{
				sb_putc(c, buf);
				c = io_getc(in);
			}
			break;
		case '"':
			quote = !quote;
			break;
		case '(':
			if	(!quote)	depth++;
			break;
		case ')':
			if	(!quote)	depth--;
			break;
		default:	
			break;
		}

		sb_putc(c, buf);
	}

	io_close(in);
	return sb2str(buf);
}


DocMac_t *Doc_getmac (Doc_t *doc, const char *name)
{
	stack_t *ptr;
	DocMac_t *mac;

	if	(!(doc && doc->cmd_stack))	return NULL;

	for (ptr = doc->cmd_stack; ptr != NULL; ptr = ptr->next)
		if ((mac = DocTab_getmac(ptr->data, name)) != NULL)
			return mac;

	return NULL;
}


void Doc_mac (Doc_t *doc, io_t *in)
{
	DocMac_t *mac;
	char *p;

	p = DocParseName(in, '@');

	if	(io_peek(in) == '=')
	{
		io_getc(in);
		p = mstrcpy(p);	/* Temporärer Buffer !!! */
		DocTab_setmac(doc->cmd_stack->data, p,
			Doc_lastcomment(doc), DocParseExpr(in));
		return;
	}

	mac = Doc_getmac(doc, p);

	if	(mac == NULL)
	{
		io_message(in, MSG_DOC, 13, 1, p);
		io_puts(p, doc->out);
		Doc_char(doc, ';');
		return;
	}

	if	(io_peek(in) == '(')
	{
		io_getc(in);
		vb_clean(&arg_buf, NULL);
		nextarg = mstrcpy(mac->name);

		do	nextarg = DocParseMacArg(in);
		while	(io_getc(in) == ',');

		p = mac_expand(mac->fmt, arg_buf.used, arg_buf.data);
		vb_clean(&arg_buf, memfree);
	}
	else	p = mac_expand(mac->fmt, 1, &mac->name);

	io_push(in, io_mstr(p));
}
