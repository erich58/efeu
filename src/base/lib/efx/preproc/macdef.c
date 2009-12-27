/*	Makrodefinition
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/preproc.h>

static void getmacarg (io_t *io, Macro_t *mac);

static iocpy_t mdef_repl[] = {
	{ "/", "\n", 1, iocpy_cskip },
	{ "\\", "!", 0, iocpy_esc },
	{ "\n", NULL, 0, NULL },
};


static iocpy_t mdef_arg[] = {
	{ "/", "\n", 1, iocpy_cskip },
	{ "\\", "!", 0, iocpy_esc },
	{ "%s",	NULL, 0, iocpy_repl },
	{ ",)", NULL, 0, NULL },
};


Macro_t *ParseMacro(io_t *io)
{
	Macro_t *mac;
	int c;

	mac = NewMacro();
	mac->name = io_mgets(io, "!%n_");
	c = io_getc(io);

	if	(c == '(')
	{
		getmacarg(io, mac);
		mac->sub = macsub_subst;
	}
	else
	{
		mac->sub = macsub_repl;
		io_ungetc(c, io);
	}

	io_eat(io, " \t");
	mac->repl = miocpy(io, mdef_repl, tabsize(mdef_repl));
	return mac;
}


static void getmacarg(io_t *io, Macro_t *mac)
{
	Macro_t *x;
	io_t *tmp;
	int c;
	int i;

	tmp = io_tmpbuf(0);
	mac->hasarg = 1;
	mac->dim = 0;
	PushMacroTab(NULL);

	while ((c = io_eat(io, "%s")) != EOF)
	{
		if	(c != ',' && c != ')')
		{
			x = NewMacro();
			x->name = miocpy(io, mdef_arg, tabsize(mdef_arg));
			x->sub = macsub_repl;
			AddMacro(x);
		}
		else	x = NULL;

		io_write(tmp, &x, sizeof(Macro_t *));
		mac->dim++;
		c = io_getc(io);

		if	(c == ')')	break;
	}

	if	(mac->dim != 0)
	{
		mac->tab = PopMacroTab();
		mac->arg = ALLOC(mac->dim, Macro_t *);
		io_rewind(tmp);

		for (i = 0; i < mac->dim; i++)
		{
			io_read(tmp, mac->arg + i, sizeof(Macro_t *));
		}

		i = mac->dim - 1;

		if	(mac->arg[i] && strcmp("...", mac->arg[i]->name) == 0)
		{
			memfree(mac->arg[i]->name);
			mac->arg[i]->name = "va_list";
		}
	}

	io_close(tmp);
}
