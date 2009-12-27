/*	Kopieren eines Strings mit Speicherzuweisung
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.6
*/

#include <EFEU/mstring.h>

/*
Die Funktion |$1| kopiert den String <s>.
Wurde als Argument ein Nullpointer �bergeben, liefert die Funktion
ebenfalls einen Nullpointer.
*/

char *mstrcpy (const char *s)
{
	return s ? mstrncpy(s, strlen(s)) : NULL;
}

/*
Die Funktion |$1| kopiert
maximal <len> Zeichen des Strings <s>. Wurde als Argument ein Nullpointer
�bergeben, liefert die Funktion einen Leerstring.
*/

char *mstrncpy (const char *s, size_t len)
{
	char *a;
	size_t i;

	a = (char *) memalloc(len + 1);
	i = 0;

	if	(s != NULL)
		while (i < len && *s != 0)
			a[i++] = *(s++);

	a[i] = 0;
	return a;
}

/*
$Notes
Das Speicherfeld f�r den R�ckgabewert wird mit |memalloc| eingerichtet.
$SeeAlso
\mref{memalloc(3)}.\br
\mref{string(3C)} im @PRM.
*/
