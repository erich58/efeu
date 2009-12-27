/*	Speicherinhalt austauschen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/memalloc.h>

/*
Die Funktion |memswap| vertauscht den Inhalt der Speicherfelder
beginnend bei <ap> und <bp>. Die Variable <n> gibt die Zahl der 
Bytes an, die vertauscht werden sollen.

$Warning
Falls sich die beiden Speicherfelder überlappen, ist das Resultat
unbestimmt.
*/

void memswap(void *ap, void *bp, size_t n)
{
	register uchar_t *a, *b;
	register uchar_t c;

	a = ap;
	b = bp;

	while (n--)
	{
		c = *a;
		*a++ = *b;
		*b++ = c;
	}
}

/*
$SeeAlso
\mref{alloctab(3)}, \mref{memalloc(3)}.\br
\mref{malloc(3S)} im @PRM.
*/
