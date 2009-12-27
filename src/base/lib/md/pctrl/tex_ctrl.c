/*	Ausgabekontrolle für TeX
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pctrl.h>
#include <EFEU/texutil.h>

static int quote_flag = 0;
static TeXfont_t *font = NULL;
static TeXpgfmt_t *pgfmt = NULL;

int tex_ctrl(PrFilter_t *pf, int cmd, va_list list)
{
	if	(!pgfmt)	pgfmt = TeXpgfmt(pctrl_pgfmt);
	if	(!font)		font = TeXfont(pctrl_fsize);

	switch (cmd)
	{
	case PCTRL_COLUMNS:

		return (int) (pgfmt->width / font->width);

	case PCTRL_LINES:

		return (int) (pgfmt->height / font->height);

	case PCTRL_BEGIN:

		pgfmt = TeXpgfmt(pctrl_pgfmt);
		font = TeXfont(pctrl_fsize);
		pf->delim = " & ";
		TeXbegin(pf->io, pgfmt);
		io_putc('\\', pf->io);
		io_puts(font->name, pf->io);
		io_putc('\n', pf->io);
		pf->put = NULL;
		break;

	case PCTRL_DATA:

		io_puts("\n\\begin{tabular}{|l|", pf->io);
		io_printf(pf->io, "*{%d}{r|}", va_arg(list, int));
		io_puts("}\n\\hline\n", pf->io);
		pf->nl = " \\\\\n";
		break;

	case PCTRL_EHEAD:

		io_puts("\\hline\n", pf->io);
		break;

	case PCTRL_EDATA:

		io_puts("\\hline\n", pf->io);
		io_puts("\\end{tabular}\n", pf->io);
		pf->nl = "~\\\\\n";
		break;

	case PCTRL_END:

		TeXend(pf->io);
		break;

	case PCTRL_LEFT:
	case PCTRL_CENTER:
	case PCTRL_RIGHT:
	case PCTRL_EMPTY:
	
		quote_flag = 0;
		pf->put = tex_put;
		break;

	case PCTRL_VALUE:

		quote_flag = 0;
		pf->put = std_put;
		break;

	default:	

		break;
	}

	return 0;
}

int tab_ctrl(PrFilter_t *pf, int cmd, va_list list)
{
	TeXcompletion = 0;
	return tex_ctrl(pf, cmd, list);
}


int tex_put(int c, PrFilter_t *pf)
{
	switch (c)
	{
	case '%': case '$': case '#': case '_':
	case '&': case '{': case '}':

		io_putc('\\', pf->io);
		io_putc(c, pf->io);
		break;

	case '^': case '~':

		io_putc('\\', pf->io);
		io_putc(c, pf->io);
		io_putc('~', pf->io);
		break;

	case '|': case '[': case ']': case '<': case '>':

		io_putc('$', pf->io);
		io_putc(c, pf->io);
		io_putc('$', pf->io);
		break;

	case '"':

		quote_flag = !quote_flag;
		io_puts(quote_flag ? "\"`" : "\"´", pf->io);
		break;

	case '\\': io_puts("$\\backslash$", pf->io); break;
	case '*':  io_puts("$\\ast$", pf->io); break;

	case 'Ä': io_puts("\"A", pf->io); break;
	case 'Ö': io_puts("\"O", pf->io); break;
	case 'Ü': io_puts("\"U", pf->io); break;
	case 'ä': io_puts("\"a", pf->io); break;
	case 'ö': io_puts("\"o", pf->io); break;
	case 'ü': io_puts("\"u", pf->io); break;
	case 'ß': io_puts("\"s", pf->io); break;
	case EOF:	break;

	default:

		if	(c < ' ' || c >= 127)
		{
			io_puts("$\\bullet$", pf->io);
		}
		else	io_putc(c, pf->io);

		break;
	}

	return c;
}
