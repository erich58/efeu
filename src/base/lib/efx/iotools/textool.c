/*	Hilfsprogramme für TeX
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/efio.h>


/*	String ausgeben
*/

int TeXputs(const char *str, io_t *io)
{
	int size;

	if	(str == NULL)	return 0;

	for (size = 0; *str != 0; str++)
		size += TeXputc(*str, io);

	return size;
}

/*	String unter Anführung ausgeben
*/

int TeXquote(const char *str, io_t *io)
{
	int size;

	size = io_puts("\"`", io);
	size += TeXputs(str, io);
	size += io_puts("\"'", io);
	return size;
}


/*	Zeichen ausgeben
*/

int TeXputc(int c, io_t *io)
{
	if	(c == EOF)	return 0;

	io = io_count(io_refer(io));

	switch ((char) c)
	{
	case EOF:
	
		break;

	case '%': case '$': case '#': case '_':
	case '&': case '{': case '}':

		io_putc('\\', io);
		io_putc(c, io);
		break;

	case '^':
	case '~':

		io_putc('\\', io);
		io_putc(c, io);
		io_putc('~', io);
		/*
		io_printf(io, "{\\tt\\symbol{%d}}", c);
		*/
		break;

	case '|': case '<': case '>':

		io_putc('$', io);
		io_putc(c, io);
		io_putc('$', io);
		break;

	case '"':

		io_printf(io, "{\\tt\\symbol{%d}}", c);
		break;

	case '\\':

		io_puts("$\\backslash$", io);
		break;

	case '-':

		io_puts("{-}", io);
		break;

	case '*':

		io_puts("$\\ast$", io);
		break;

	case '\t': case '\n':
	case 'Ä': case 'Ö': case 'Ü':
	case 'ä': case 'ö': case 'ü': case 'ß':

		io_putc(c, io);
		break;

	default:

		if	(c < ' ' || c >= 127)
		{
			io_puts("$\\bullet$", io);
		}
		else	io_putc(c, io);

		break;
	}

	return io_close(io);
}
