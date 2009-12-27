/*	String/Zeichen mit Sonderdarstellung ausgeben
	(c) 1993 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6

	Version 0.4
*/


#include <EFEU/io.h>
#include <EFEU/patcmp.h>
#include <ctype.h>

#define	ESCEXT	0	/* \e=^[ und \d=^? - Erweiterung verwenden */
#define	LATIN1	1	/* Zeichen 161 - 255 unver�ndert ausgeben */

#if	LATIN1
#define	printable(c)	isprint(c) || ((uchar_t) (c) > 160)
#else
#define	printable(c)	isprint(c)
#endif


/*	String von Sonderdarstellung ausgeben
*/

int io_xputs(const char *str, io_t *io, const char *delim)
{
	if	(str != NULL)
	{
		const uchar_t *p = (const uchar_t *) str;
		int n;

		for (n = 0; *p != 0; p++)
			n += io_xputc(*p, io, delim);

		return n;
	}

	return 0;
}


/*	Zeichen mit Sonderdarstellung ausgeben
*/

int io_xputc(int c, io_t *io, const char *delim)
{
	int pos;
	int flag;
	char buf[8];

	pos = 0;
	flag = 1;
	c = (uchar_t) c;

	switch (c)
	{
	case '\b':	c = 'b'; break;
	case '\f':	c = 'f'; break;
	case '\n':	c = 'n'; break;
	case '\r':	c = 'r'; break;
	case '\t':	c = 't'; break;
	case '\v':	c = 'v'; break;
	case '\\':	break;
	case '^':	break;
	case '~':	break;
#if	ESCEXT
	case 033:	c = 'e'; break;
	case 127:	c = 'd'; break;
#endif
	default:	flag = listcmp(delim, c); break;
	}

	if	(flag)
	{
		buf[pos++] = '\\';
		buf[pos++] = (char) c;
	}
	else if	(printable(c))
	{
		buf[pos++] = (char) c;
	}
	else
	{
		if	(c & 0x80)
		{
			buf[pos++] = '~';
			c &= 0x7F;
		}

		if	(c == (c & 0x1F))
		{
			buf[pos++] = '^';
			buf[pos++] = '@' | c;
		}
		else if	(c == 127)
		{
			buf[pos++] = '^';
			buf[pos++] = '?';
		}
		else if	(listcmp(delim, c))
		{
			buf[pos++] = '\\';
			buf[pos++] = c;
		}
		else	buf[pos++] = c;
	}
	
	buf[pos] = 0;
	return io_puts(buf, io);
}
