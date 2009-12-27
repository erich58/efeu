/*	Identifikationsstring einer IO-Struktur generieren
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/mstring.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>

char *io_xident (io_t *io, const char *fmt, ...)
{
	strbuf_t *buf;
	char *id, *p;
	va_list list;

	id = io_ident(io);

	if	(fmt == NULL)	return id;

	list = va_start(list, fmt);
	buf = new_strbuf(0);

	for (; *fmt != 0; fmt++)
	{
		if	(*fmt == '%')
		{
			fmt++;

			switch (*fmt)
			{
			case 'd':
				sb_vprintf(buf, "%d", list);
				break;
			case 's':
				p = va_arg(list, char *);
				sb_puts(p, buf);
				break;
			case 'm':
				p = va_arg(list, char *);
				sb_puts(p, buf);
				memfree(p);
				break;
			case '*':
				sb_puts(id, buf);
				break;
			case 0:
				break;
			default:
				sb_putc(*fmt, buf);
				break;
			}
		}
		else	sb_putc(*fmt, buf);
	}

	va_end(list);
	memfree(id);
	return sb2str(buf);
}
