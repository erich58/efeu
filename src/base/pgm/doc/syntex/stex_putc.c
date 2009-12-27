/*	Ausgabefilter für SynTeX
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <SynTeX.h>
#include <efeudoc.h>
#include <ctype.h>


static void stex_filter (int c, io_t *io)
{
	if	(c == EOF)	return;

	c = (uchar_t) c;

	switch (c)
	{
	case '%': case '$': case '#': case '_':
	case '&': case '{': case '}':

		io_putc('\\', io);
		io_putc(c, io);
		break;

	case '^': case '~': case '\\':
	case '|': case '<': case '>': case '"':

		io_printf(io, "\\texttt{\\char%d}", c);
		break;

	case '-': 
		io_puts("{-}", io);
		break;

	case '\t': case '\n':
		io_putc(c, io);
		break;

	case 0xa1:	io_puts("!", io); break;
	case 0xa7:	io_puts("{\\textsection}", io); break;
	case 0xa9:	io_puts("{\\textcopyright}", io); break;
	case 0xaa:	io_puts("{\\tt\\textordfeminine}", io); break;
	case 0xac:	io_puts("{\\textlnot}", io); break;
	case 0xad:	io_puts("{-}", io); break;
	case 0xaf:	io_puts("{\\textasciimacron}", io); break;
	case 0xb1:	io_puts("{\\textpm}", io); break;
	case 0xb2:	io_puts("{\\texttwosuperior}", io); break;
	case 0xb3:	io_puts("{\\textthreesuperior}", io); break;
	case 0xb5:	io_puts("{\\textmu}", io); break;
	case 0xb6:	io_puts("{\\textparagraph}", io); break;
	case 0xb9:	io_puts("{\\textonesuperior}", io); break;
	case 0xba:	io_puts("{\\tt\\textordmasculine}", io); break;
	case 0xbf:	io_puts("?", io); break;
	case 0xd0:	io_putc('D', io); break;
	case 0xd7:	io_puts("{\\texttimes}", io); break;
	case 0xde:	io_puts("T", io); break;
	case 0xf0:	io_putc('d', io); break;
	case 0xf7:	io_puts("{\\textdiv}", io); break;
	case 0xfe:	io_puts("t", io); break;

	default:

		if	(c < ' ' || (c >= 127 && c < 160))
		{
			io_puts("$\\bullet$", io);
		}
		else	io_putc(c, io);

		break;
	}
}


int SynTeX_plain (SynTeX_t *stex, int c)
{
	if	(c == ' ' && stex->last == '\n')
		io_puts("\\strut~", stex->out);
	else	stex_filter(c, stex->out);

	return c;
}

int SynTeX_putc (SynTeX_t *stex, int c)
{
	if	(stex->ignorespace && isspace(c))
		return c;

	stex->ignorespace = 0;
	stex_filter(c, stex->out);
	return c;
}


int SynTeX_xputc (SynTeX_t *stex, int c)
{
	if	(isspace(c))
	{
		if	(stex->ignorespace)	return 0;

		stex->ignorespace = 1;
		stex->space = c;
		return c;
	}

	if	(stex->space)
		io_putc(stex->space, stex->out);

	stex->ignorespace = 0;
	stex->space = 0;
	stex_filter(c, stex->out);
	return c;
}

void SynTeX_puts (SynTeX_t *stex, const char *str)
{
	if	(str)
	{
		while (*str != 0)
			stex_filter(*(str++), stex->out);
	}
}

void SynTeX_newline (SynTeX_t *stex)
{
	if	(stex->last != '\n')
		io_putc('\n', stex->out);

	stex->last = '\n';
}

void SynTeX_rem (SynTeX_t *stex, const char *cmd)
{
	io_putc('%', stex->out);

	if	(cmd)
	{
		for (; *cmd != 0; cmd++)
		{
			io_putc(*cmd, stex->out);

			if	(*cmd == '\n')	io_putc('%', stex->out);
		}
	}

	io_putc('\n', stex->out);
}
