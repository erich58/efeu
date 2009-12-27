/*	Zeichen mit Sonderbedeutung lesen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/patcmp.h>
#include <ctype.h>

static int get_oct (io_t *io);
static int get_hex (io_t *io);


/*	Zeichen lesen
*/

int io_xgetc (io_t *io, const char *delim)
{
	int c, meta;

	meta = 0;

	while ((c = io_getc(io)) != EOF)
	{
	/*	Escape - Sequenz
	*/
		if	(c == '\\')
		{
			switch ((c = io_getc(io)))
			{
			case EOF:	return EOF;
			case '\n':	io_linemark(io); continue;
			case '~':
			case '^':
			case '\\':	return (c | meta);
			case 'b':	return ('\b' | meta);
			case 'f':	return ('\f' | meta);
			case 'n':	return ('\n' | meta);
			case 'r':	return ('\r' | meta);
			case 't':	return ('\t' | meta);
			case 'v':	return ('\v' | meta);
			case '0':
			case '1':
			case '2':
			case '3':	io_ungetc(c, io);
					return (get_oct(io) | meta);
			case 'x':	return (get_hex(io) | meta);
			case 'e':	return (033 | meta);
			case 'd':	return (127 | meta);
			default:	break;
			}

			if	(listcmp(delim, c))
				return (c | meta);

			io_ungetc(c, io);
			return ('\\' | meta);
		}

	/*	Sonstige Zeichen
	*/
		else if	(listcmp(delim, c) && !meta)
		{
			io_ungetc(c, io);
			break;
		}
		else if	(c == '^')
		{
			switch ((c = io_getc(io)))
			{
			case EOF:	return EOF;
			case '?':	return 127 | meta;
			default:	return (c & 0x1F) | meta;
			}

			break;
		}
		else if	(c == '~' && !meta)
		{
			meta = 0x80;
		}
		else	return (c | meta);
	}

	return EOF;
}


/*	Oktales Zeichen lesen
*/

static int get_oct (io_t *io)
{
	char buf[4];
	int i, c;

	for (i = 0; i < 3; i++)
	{
		c = io_getc(io);

		if	(c == EOF)
		{
			break;
		}
		else if	(!isdigit(c) || c == '8' || c == '9')
		{
			io_ungetc(c, io);
			break;
		}
		else	buf[i] = (char) c;
	}

	buf[i] = 0;
	return (int) strtol(buf, NULL, 8) & 0xFF;
}


/*	Hexadezimales Zeichen lesen
*/

static int get_hex (io_t *io)
{
	char buf[3];
	int i, c;

	for (i = 0; i < 2; i++)
	{
		c = io_getc(io);

		if	(c == EOF)
		{
			break;
		}
		else if	(!isxdigit(c))
		{
			io_ungetc(c, io);
			break;
		}
		else	buf[i] = (char) c;
	}

	buf[i] = 0;
	return (int) strtol(buf, NULL, 16);
}
