/*	Steuerbefehle
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mstring.h>
#include <EFEU/procenv.h>
#include <EFEU/parsub.h>
#include <EFEU/ioctrl.h>
#include <EFEU/patcmp.h>
#include <DocCtrl.h>
#include <SynTeX.h>
#include <efeudoc.h>
#include <ctype.h>

static void label_cmd (SynTeX_t *stex, const char *cmd, const char *name)
{
	if	(name)
	{
		io_puts(cmd, stex->out);
		io_putc('{', stex->out);

		for (; *name != 0; name++)
		{
			if	(isalnum(*name) || strchr(".:/-", *name))
				io_putc(*name, stex->out);
			else	io_printf(stex->out, "[%02X]", *name);
		}

		io_putc('}', stex->out);
	}
}

static int index_mark (int c)
{
	switch ((char) c)
	{
	case '!': case '@':
	case 'Ä': case 'Ö': case 'Ü':
	case 'ä': case 'ö': case 'ü': case 'ß':
		return 1;
	default:
		return 0;
	}
}

static void put_index (SynTeX_t *stex, va_list list)
{
	char *name = va_arg(list, char *);
	char *label = va_arg(list, char *);

	if	(!name || !label)	return;

	SynTeX_puts(stex, lexsortkey(name, NULL));
	io_putc('@', stex->out);

	for (; *label != 0; label++)
	{
		if	(index_mark(*label))
			io_putc('"', stex->out);

		io_putc(*label, stex->out);
	}
}

static void add_space (SynTeX_t *stex, int space)
{
	io_putc(space, stex->out);
	stex->last = space;
	stex->ignorespace = 1;
}

static void line_cmd (SynTeX_t *stex, const char *cmd)
{
	SynTeX_newline(stex);
	io_puts(cmd, stex->out);
	add_space(stex, '\n');
}


int SynTeX_cmd (SynTeX_t *stex, va_list list)
{
	int cmd;
	char *p;
	
	cmd = va_arg(list, int);

	switch (cmd)
	{
	case DOC_CMD_BREAK:
		io_puts("\\\\", stex->out);
		break;
	case DOC_CMD_NPAGE:
		line_cmd(stex, "\\newpage");
		break;
	case DOC_CMD_TOC:
		line_cmd(stex, "\\tableofcontents");
		break;
	case DOC_CMD_LOF:
		line_cmd(stex, "\\listoffigures");
		break;
	case DOC_CMD_APP:
		SynTeX_newline(stex);
		io_puts("\\appendix", stex->out);

		if	((p = va_arg(list, char *)) != NULL)
			io_printf(stex->out, "[%s]", p);

		add_space(stex, '\n');
		break;
	case DOC_CMD_ITEM:
		line_cmd(stex, "\\item");
		break;
	case DOC_CMD_MARK:
		io_printf(stex->out, "\\fn{%d}", va_arg(list, int));
		break;
	case DOC_CMD_IDX:
		if	(stex->last != '\n')
			io_puts("%\n", stex->out);

		io_puts("\\index{", stex->out);
		put_index(stex, list);
		io_puts("}%", stex->out);
		add_space(stex, '\n');
		break;
	case DOC_CMD_TEX:
		io_puts(va_arg(list, char *), stex->out);
		break;
	case DOC_REF_LABEL:
		label_cmd(stex, "\\label", va_arg(list, char *));
		SynTeX_rem(stex, NULL);
		break;
	case DOC_REF_STD:
		label_cmd(stex, "\\protect\\ref", va_arg(list, char *));
		break;
	case DOC_REF_PAGE:
		label_cmd(stex, "\\protect\\pageref", va_arg(list, char *));
		break;
	case DOC_REF_VAR:
		label_cmd(stex, "\\protect\\vref", va_arg(list, char *));
		break;
	case DOC_TAB_SEP:
		io_puts(" &", stex->out);
		add_space(stex, ' ');
		break;
	case DOC_TAB_END:
		io_puts("\\cr", stex->out);
		add_space(stex, '\n');
		break;
	case DOC_TAB_HEIGHT:
		SynTeX_newline(stex);
		io_printf(stex->out, "\\lineheight{%d}", va_arg(list, int));
		add_space(stex, '\n');
		break;
	case DOC_TAB_INDENT:
		io_puts("\\indent", stex->out);
		add_space(stex, ' ');
		break;
	case DOC_TAB_BLINE:
		line_cmd(stex, "\\boldline");
		break;
	case DOC_TAB_BRULE:
		line_cmd(stex, "\\boldrule");
		break;
	default:
		return EOF;
	}

	return 0;
}
