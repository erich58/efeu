/*	Zusammenfügen von Strings
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>

/*
Die Funktion |$1| dient zum Zusammenhängen beliebig vieler Strings
mit Trennzeichen <delim>.
Die variable Argumentliste wird mit einem Nullpointer abgeschlossen.
*/

char *mstrcat(const char *delim, const char *list, ...)
{
	const char *x;
	char *s;
	strbuf_t *sb;
	va_list k;

	sb = new_strbuf(0);
	va_start(k, list);
	x = NULL;

	for (s = (char *) list; s != NULL; s = va_arg(k, char *))
	{
		sb_puts(x, sb);
		sb_puts(s, sb);
		x = delim;
	}

	va_end(k);
	return sb2str(sb);
}


/*
Die Funktion <$1> hängt die beiden Strings <a> und <b>
mit dem Trennzeichen <delim> zusammen. Falls für einen beiden Strings
ein Nullpointer übergeben wurde, wird kein Trennzeichen ausgegeben.
Sind beide Strings Nullpointer, liefert die Funktion einen Nullpointer.
*/

char *mstrpaste(const char *delim, const char *a, const char *b)
{
	strbuf_t *sb;

	if	(a == NULL)	return mstrcpy(b);
	if	(b == NULL)	return mstrcpy(a);

	sb = new_strbuf(0);
	sb_puts(a, sb);
	sb_puts(delim, sb);
	sb_puts(b, sb);
	return sb2str(sb);
}

/*
$Notes
Das Speicherfeld für den Rückgabewert wird mit |memalloc| eingerichtet.
$SeeAlso
\mref{memalloc(3)}.\br
\mref{string(3C)} im @PRM.
*/
