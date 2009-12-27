/*	Verwalten, Suchen und Sortieren von Eintr�gen in einem Vektorbuffer
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.6
*/

#include <EFEU/vecbuf.h>

static void move (char *a, char *b, size_t n)
{
	while (n-- > 0)
	{
		*a = *b;
		*b = 0;
		a++;
		b++;
	}
}

/*
Die Funktion |$1| dient zum Verwalten von Eintr�gen
in einem sortierten Vektorbuffer. Der Suchschl�ssel <data>
dient zur Suche von Eintr�gen und als Zwischenbuffer f�r
Daten. Bei neuen Eintr�gen werden die Daten vom Suchschl�ssel
in den Vektorbuffer �bertragen, bei gel�schten Eintr�gen
werden die Daten in den Suchschl�ssel geschrieben.

Die Vergleichsfunktion <comp> wird mit zwei Pointern
auf Datenelemente aufgerufen und mu� einen Wert
kleiner, gleich oder gr��er 0 liefern, je nachdem ob
der erste Datenwert kleiner, gleich oder gr��er als der zweite
Datenwert ist.

Als Modus <mode> ist eines der folgenden Werte zul�ssig:

[|VB_SEARCH|]
	Falls der Eintrag gefunden wurde, liefert die Funktion
	den Pointer auf den Eintrag, ansonsten einen Nullpointer.
	Der Suchschl�ssel wird nicht ver�ndert.
[|VB_ENTER|]
	Falls der Eintrag noch nicht vorhanden ist,
	wird ein neuer Eintrag generiert
	und die Daten vom Suchschl�ssel �bernommen.
	Der Suchschl�ssel wird anschlie�end gel�scht.
	Die Funktion liefert den Pointer auf den (neuen) Eintrag.
[|VB_REPLACE|]
	Falls der Eintrag bereits vorhanden ist,
	werden die Daten mit dem Suchschl�ssel ausgetauscht,
	und die Funktion liefert den Pointer auf den Suchschl�ssel.
	Ansonsten wird ein neuer Entrag generiert
	und die Daten vom Suchschl�ssel �bernommen.
	Der Suchschl�ssel wird anschlie�end gel�scht
	und die Funktion liefert einen Nullpointer.
[|VB_DELETE|]
	Falls der Eintrag in der Tabelle gefunden wird,
	wird er gel�scht. Die Datenwerte werden in den Suchschl�ssel
	�bertragen und die Funktion liefert
	den Pointer auf den Suchschl�ssel.
	Ansonsten wird ein Nullpointer zur�ckgegeben.
*/

void *vb_search (vecbuf_t *buf, void *data, comp_t comp, int mode)
{
	char *p1;
	int i, i1, i2, v;

	if	(!buf)	return NULL;

/*	Suche nach Eintrag
*/
	i2 = buf->used;
	i1 = comp ? 0 : i2;

	while (i1 != i2)
	{
		i = (i1 + i2) / 2;
		p1 = (char *) buf->data + i * buf->elsize;
		v = comp(data, p1);

		if	(v > 0)	i1 = i + 1;
		else if	(v < 0)	i2 = i;
		else	
		{
			switch (mode)
			{
			case VB_ENTER:
			case VB_SEARCH:

				return p1;

			case VB_DELETE:

				memcpy(data, p1, buf->elsize);
				vb_delete(buf, i, 1);
				return data;


			case VB_REPLACE:

				{
					register uchar_t *a = data;
					register uchar_t *b = (uchar_t *) p1;
					register size_t n = buf->elsize;

					while (n--)
					{
						register uchar_t c = *a;
						*a++ = *b;
						*b++ = c;
					}
				}

				return data;

			default:

				break;
			}

			i2 = i + 1;
			break;
		}
	}

/*	Datenwert �bernehmen
*/
	if	(mode == VB_ENTER || mode == VB_REPLACE)
	{
		p1 = vb_insert(buf, i2, 1);
		move(p1, data, buf->elsize);

		if	(mode == VB_ENTER)	return p1;
	}

	return NULL;
}

/*
Die Funktion |$1| sortiert die Daten eines Vektorbuffers
mit der Bibliotheksfunktion |qsort|.
*/

void vb_qsort (vecbuf_t *buf, comp_t comp)
{
	if	(buf && buf->used > 1)
		qsort(buf->data, buf->used, buf->elsize, comp);
}

/*
Die Funktion |$1| verwendet die Bibliotheksfunktion
|bsearch| zur Suche eines Eintrages und ist �quivalent zu
|vb_search| mit Flag |VB_SEARCH|.
*/

void *vb_bsearch (vecbuf_t *buf, const void *key, comp_t comp)
{
	if	(buf && buf->used > 0)
		return bsearch(key, buf->data, buf->used, buf->elsize, comp);
	else	return NULL;
}

/*
$SeeAlso
\mref{vecbuf(7)}, \mref{types(7)}.\br
\mref{bsearch(3C)}, \mref{qsort(3C)} im @PRM
*/
