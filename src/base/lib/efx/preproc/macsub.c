/*	Makrosubstitution
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/preproc.h>
#include <EFEU/pconfig.h>
#include <EFEU/ioctrl.h>
#include <ctype.h>

static iocpy_t mcpy_delim[] = {
	{ "/", "\n", 1, iocpy_cskip },
	{ "\n", NULL, 0, NULL },
	{ "!%s", NULL, 0, NULL },
};


static iocpy_t mcpy_mac[] = {
	{ "/", "\n", 1, iocpy_cskip },
	{ "\\", "#", 0, iocpy_esc },
	{ "\\", "!", 1, iocpy_esc },
	{ "\n", NULL, 0, NULL },
	{ "%a_", "!%n_", 0, iocpy_macsub },
	{ "\"", "\"", 1, iocpy_str },
	{ "'",	"'", 1, iocpy_str },
};

static iocpy_t mcpy_mac2[] = {
	{ "/", "\n", 1, iocpy_cskip },
	{ "\\", "#", 0, iocpy_esc },
	{ "\\", "!", 1, iocpy_esc },
	{ "\n", NULL, 0, NULL },
	{ "%a_", "!%n_", 0, iocpy_macsub },
	{ "\"", "\"", 1, iocpy_strmacsub },
	{ "'",	"'", 1, iocpy_strmacsub },
};


int io_macsub(io_t *in, io_t *out, const char *delim)
{
	int c;
	int n;

	n = 0;

	while ((c = io_egetc(in, delim)) != EOF)
	{
		if	(isalpha(c) || c == '_')
		{
			n += iocpy_macsub(in, out, c, "!%n_", 1);
		}
		else if	(c =='"')
		{
			n += iocpy_str(in, out, c, "\"", 1);
		}
		else if	(c =='\'')
		{
			n += iocpy_str(in, out, c, "'", 1);
		}
		else if	(io_putc(c, out) != EOF)
		{
			n++;
		}
	}

	return n;
}


int io_fmacsub(const char *fmt, io_t *out)
{
	return iocpyfmt(fmt, out, mcpy_mac, tabsize(mcpy_mac));
}

char *macsub(const char *fmt)
{
	return miocpyfmt(fmt, mcpy_mac, tabsize(mcpy_mac));
}



int macsub_repl (Macro_t *mac, io_t *io, char **arg, int narg)
{
	return iocpyfmt(mac->repl, io, mcpy_mac2, tabsize(mcpy_mac2));
}

int macsub_def (Macro_t *mac, io_t *io, char **arg, int narg)
{
	return io_puts(GetMacro(arg[0]) ? "1" : "0", io);
}


int macsub_subst (Macro_t *mac, io_t *io, char **arg, int narg)
{
	int i, n;

	for (i = 0; i < mac->dim; i++)
	{
		if	(mac->arg[i] == NULL)	continue;
		if	(i >= narg)		break;

		mac->arg[i]->repl = arg[i];
	}

	PushMacroTab(mac->tab);
	n = iocpyfmt(mac->repl, io, mcpy_mac2, tabsize(mcpy_mac2));
	PopMacroTab();

	for (i = 0; i < mac->dim; i++)
	{
		if	(mac->arg[i] == NULL)	continue;

		mac->arg[i]->repl = NULL;
	}

	return n;
}


int iocpy_macsub(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	char *s;
	int n;
	Macro_t *mac;
	char **macarg;
	int narg;
	int i;
	strbuf_t *sb;

	sb = new_strbuf(0);
	sb_putc(c, sb);

	while ((c = io_getc(in)) != EOF)
	{
		if	(listcmp(arg, c))
		{
			io_ungetc(c, in);
			break;
		}
		else	sb_putc(c, sb);
	}

	s = sb2str(sb);
	mac = GetMacro(s);

	if	(mac == NULL || mac->lock)
	{
		n = io_puts(s, out);
		FREE(s);
		return n;
	}

	macarg = NULL;
	narg = 0;

	if	(mac->hasarg)
	{
		char *delim;

		delim = miocpy(in, mcpy_delim, tabsize(mcpy_delim));

		if	(iocpy_last != '(')
		{
			n = io_puts(s, out);
			n += io_puts(delim, out);
			FREE(s);
			FREE(delim);
			return n;
		}

		io_getc(in);
		narg = macarglist(in, &macarg);
	}

	if	(mac->lock)
	{
		n = io_puts(s, out);
		FREE(s);
		return n;
	}

	mac->lock = 1;
	n = mac->sub(mac, out, macarg, narg);
	mac->lock = 0;

	for (i = 0; i < narg; i++)
		FREE(macarg[i]);

	FREE(macarg);
	FREE(s);
	return n;
}


int iocpy_strmacsub(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int n;
	int escape;

	n = io_nputc(c, out, 1);
	escape = 0;

	while ((c = io_mgetc(in, 0)) != EOF)
	{
		if	(escape)
		{
			escape = 0;
		}
		else if	(listcmp(arg, c))
		{
			if	(flags & 2)
			{
				int d;

				d = io_mgetc(in, 0);

				if	(d == c)
				{
					n += io_nputc(c, out, 1);
					continue;
				}
				else	io_ungetc(d, in);
			}

			n += io_nputc(c, out, 1);
			return n;
		}
		else if	(c == '_' || isalpha(c))
		{
			n += iocpy_macsub(in, out, c, "!%n_", flags);
			continue;
		}
		else	escape = (c == '\\');

		n += io_nputc(c, out, 1);
	}

	return n;
}
