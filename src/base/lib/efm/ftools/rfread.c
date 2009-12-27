/*	Binäre Ein- Ausgabe mit reverser Byteordnung
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/


#include <EFEU/ftools.h>

/*
Die Funktion |$1| liest <nitems> Datensätze der Länge <size>
aus der Datei <stream> in den Buffer <ptr>,
wobei jeder Datensatz in umgekehrter Reihenfolge gelesen.
*/

size_t rfread (void *ptr, size_t size, size_t nitems, FILE *file)
{
	nitems = fread(ptr, size, nitems, file);

	if	(size > 1)
	{
		size_t n, k;
		char c, *p;

		p = (char *) ptr;

		for (n = 0; n < nitems; n++)
		{
			for (k = (size + 1) / 2; k < size; k++)
			{
				c = p[k];
				p[k] = p[size - k - 1];
				p[size - k - 1] = c;
			}

			p += size;
		}
	}

	return nitems;
}

/*
Die Funktion |$1| schreibt <nitems> Datensätze der Länge <size>
vom Buffer <ptr> in die Datei <stream>,
wobei jeder Datensatz in umgekehrter Reihenfolge ausgegeben wird.
*/

size_t rfwrite (const void *ptr, size_t size, size_t nitems, FILE *file)
{
	if	(size > 1)
	{
		size_t n, k;
		const uchar_t *p;

		p = ptr;

		for (n = 0; n < nitems; n++)
		{
			k = size;

			while (k-- != 0)
				putc(p[k], file);

			if	(feof(file))	break;

			p += size;
		}

		return n;
	}
	else	return fwrite(ptr, size, nitems, file);
}

/*
$Note
Die Funktionen sind, abgesehen von der Bytereihenfolge eines Datensatzes,
mit |fread| und |fwrite| kompatibel.

$SeeAlso
\mref{fread(3S)} im @PRM.
*/
