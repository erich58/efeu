/*
Multibyte-String ausgeben
*/

#include <EFEU/strbuf.h>
#include <EFEU/fmtkey.h>

/*
Die Funktion |$1| liest Multibyte-Zeichen aus dem String <str> und schreibt sie
in den Stringbuffer <buf>. Die Funktion liefert die Zahl der geschriebenen
Zeichen (nicht Byte).
*/

int sb_putmb (const char *str, StrBuf *sb)
{
	if	(str && sb)
	{
		int n;

		for (n = 0; *str; n++)
			sb_putucs(pgetucs((char **) &str, 4), sb);

		return n;
	}

	return 0;
}
