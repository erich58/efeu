/*	Vergleich eines Zeichens mit einer Liste
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/patcmp.h>
#include <ctype.h>


/*	Metazeichen
*/

#define	ESCAPE(x)	(x == '\\')
#define	RANGE(x)	(x == '-')
#define	NEGATION(x)	(x == '!' || x == '^')
#define	SPECIAL(x)	(x == '%')

#define	ODIGIT(c)	(isdigit(c) && c != '8' && c != '9')


/*	Vergleich mit Zeichenliste
*/

int listcmp(const char *ptr, int c)
{
	const uchar_t *list;	/* Liste */
	int flag;		/* Negationsflag */
	int n;			/* Hilfszähler */

/*	Test auf leere Liste
*/
	if	(ptr == NULL)	return 0;

	list = (const uchar_t *) ptr;
	c = (uchar_t) c;
	flag = 1;

/*	Test auf Negationszeichen
*/
	while (NEGATION(*list))
	{
		list++;
		flag = ! flag;
	}

/*	Abarbeiten der Vergleichsliste
*/
	for (n = 0; list[n] != 0; n++)
	{
		if	(list[n + 1] == 0)
		{
			if	(c == list[n])
				return flag;
		}
		else if	(SPECIAL(list[n]))
		{
			int f;

			n++;

			switch (list[n])
			{
			case 's':	f = isspace(c); break;
			case 'p':	f = ispunct(c); break;
			case 'd':	f = isdigit(c); break;
			case 'o':	f = ODIGIT(c); break;
			case 'x':	f = isxdigit(c); break;
			case 'u':	f = isupper(c); break;
			case 'l':	f = islower(c); break;
			case 'a':	f = isalpha(c); break;
			case 'n':	f = isalnum(c); break;
			case '@':	f = (c == 0); break;
			case '^':	f = iscntrl(c); break;
			case '~':	f = (c & 0x80); break;
			default:	f = (c == list[n]); break;
			}

			if	(f)	return flag;
		}
		else if	(RANGE(list[n]) && n > 0)
		{
			int a, b;

			a = list[n-1];
			n++;

			if	(ESCAPE(list[n]) && list[n+1] != 0)
				n++;

			if	(a > list[n])
			{
				b = a;
				a = list[n];
			}
			else	b = list[n];

			if	(a <= c && c <= b)
				return flag;
		}
		else
		{
			if (ESCAPE(list[n]))	n++;
			if (c == list[n])	return flag;
		}
	}

	return (!flag);
}
