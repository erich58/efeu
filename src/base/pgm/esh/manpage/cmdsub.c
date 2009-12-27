/*	Befehlssubstitution
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/efio.h>
#include <EFEU/oldstack.h>

static oldstack_t *csubstack = NULL;

static int iocpy_cmdsub(io_t *in, io_t *out, int key, const char *xarg, unsigned flags);

static iocpy_t cpydef[] = {
	{ "/",	"\n",	1, iocpy_cskip },	/* Kommentare überlesen */
	{ "$",	NULL,	0, iocpy_psub },	/* Parametersubstitution */
	{ "@",	NULL,	0, iocpy_cmdsub },	/* Befehlssubstitution */
};


int cmdsub(io_t *in, io_t *out, cmddef_t *def, size_t dim)
{
	if	(dim)
	{
		xtab_t *tab;

		tab = xcreate(dim, skey_cmp);
		xappend(tab, def, dim, sizeof(cmddef_t), XS_ENTER);
		pushstack(&csubstack, tab);
	}

	iocpy(in, out, cpydef, tabsize(cpydef));

	if	(dim)
	{
		xdestroy(popstack(&csubstack), NULL);
	}
}

static void *stacksearch(oldstack_t *stack, void *key)
{
	void *p;
	oldstack_t *x;

	if	(stack == NULL)	return NULL;

	x = stack;

	do
	{
		if	((p = xsearch(x->value, key, XS_FIND)) != NULL)
		{
			return p;
		}
		else	x = x->next;
	}
	while	(x != stack);

	return NULL;
}

static cmddef_t *getcmddef(const char *name)
{
	cmddef_t key;

	key.name = (char *) name;
	return stacksearch(csubstack, &key);
}


/*	Befehlsargument lesen
*/

static char *getcmdarg(io_t *io, char *mask)
{
	strbuf_t *sb;
	int depth;
	int c;

	sb = new_strbuf(32);
	depth = 0;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == mask[2])
		{
			sb_putc(c, sb);
			c = io_getc(io);

			if	(c == EOF)	break;
		}
		else if	(c == mask[1])
		{
			if	(depth == 0)
			{
				break;
			}
			else	depth--;
		}
		else if	(c == mask[0])
		{
			depth++;
		}

		sb_putc(c, sb);
	}

	sb_putc(0, sb);
	return sb2mem(sb);
}


/*	Befehlsargument ausgeben
*/

static int putcmdarg(const char *str, io_t *io, char *mask)
{
	int n;
	int depth;

	depth = 0;
	n = io_nputc(mask[0], io, 1);

	while (*str != 0)
	{
		if	(*str == mask[2])
		{
			n += io_nputc(*str, io, 1);

			if	(str[1])	str++;
		}
		else if	(*str == mask[1])
		{
			if	(depth == 0)
			{
				n += io_nputc(mask[2], io, 1);
			}
			else	depth--;
		}
		else if	(*str == mask[0])
		{
			depth++;
		}

		n += io_nputc(*str, io, 1);
		str++;
	}

	n += io_nputc(mask[1], io, 1);
	return n;
}


/*	Befehlssubstitution durchführen
*/

static int iocpy_cmdsub(io_t *in, io_t *out, int key, const char *xarg, unsigned flags)
{
	strbuf_t *sb;
	int c;
	cmddef_t *def;
	int n, i;
	char *name;
	char *opt;

/*	Befehlsnamen bestimmen
*/
	sb = new_strbuf(32);
	c = io_getc(in);
	sb_putc(c, sb);

	if	(isalpha(c) || c == '_')
	{
		while ((c = io_getc(in)) != EOF)
		{
			if	(isalnum(c) || c == '_')
			{
				sb_putc(c, sb);
			}
			else	break;
		}
	}
	else	c = io_getc(in);

	name = sb2str(sb);

/*	Option abfragen
*/
	if	(c == '{')
	{
		opt = getcmdarg(in, "{}\\");
	}
	else
	{
		while (c == ' ' || c == '\t')
			c = io_getc(in);

		io_ungetc(c, in);
		opt = flags ? io_mgets(in, "\n") : NULL;
	}

/*	Befehl abfragen
*/
	def = getcmddef(name);

	if	(def == NULL)
	{
		n = io_nputc(key, out, 1);
		n += io_puts(name, out);

		if	(opt)
		{
			n += putcmdarg(opt, out, "{}\\");
		}
		else	n += io_nputc(' ', out, 1);
	}
	else if	(def->eval)
	{
		n = def->eval(out, opt, def->par);
	}
	else	n = 0;

	memfree(name);
	memfree(opt);
	return n;
}
