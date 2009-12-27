/*	Formatierungshifsprogramme
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/fmtkey.h>
#include <EFEU/locale.h>

void ftool_addsig (strbuf_t *sb, int sig, int flags);
int ftool_ioalign (io_t *io, strbuf_t *sb, const fmtkey_t *key);

#define	LOCALE(name)	(Locale.print ? Locale.print->name : NULL)
#define	NEG_SIG		LOCALE(negative_sign)
#define	POS_SIG		LOCALE(positive_sign)
#define	ZERO_SIG	LOCALE(zero_sign)


/*	Vorzeichen ausgeben
*/

void ftool_addsig (strbuf_t *sb, int sig, int flags)
{
	char *p;

	if	(sig < 0)		p = NEG_SIG;
	else if	(flags & FMT_SIGN)	p = (sig > 0) ? POS_SIG : ZERO_SIG;
	else if	(flags & FMT_BLANK)	p = " ";
	else				return;

	sb_rputs(p, sb);
}


/*	Ausgeben mit Feldausrichtung
*/

int ftool_ioalign (io_t *io, strbuf_t *sb, const fmtkey_t *key)
{
	int n;

	n = 0;

	if	(key->flags & FMT_RIGHT)
		n += io_nputc(' ', io, key->width - sb->pos);

	for (; sb->pos > 0; n++)
		io_putc(sb->data[--sb->pos], io);

	n += io_nputc(' ', io, key->width - n);
	del_strbuf(sb);
	return n;
}
