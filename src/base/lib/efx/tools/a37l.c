/*	Umwandeln von Ziffern der Basis 37 in eine Zahl
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/


#include <EFEU/efutil.h>

unsigned a37l (const char *s)
{
	unsigned x;

	if	(s == NULL)	return 0;

	for (x = 0; *s != 0; s++)
	{
		x *= 37;

		switch (*s)
		{
		case '0':		x +=  1; break;
		case '1':		x +=  2; break;
		case '2':		x +=  3; break;
		case '3':		x +=  4; break;
		case '4':		x +=  5; break;
		case '5':		x +=  6; break;
		case '6':		x +=  7; break;
		case '7':		x +=  8; break;
		case '8':		x +=  9; break;
		case '9':		x += 10; break;
		case 'A': case 'a':	x += 11; break;
		case 'B': case 'b':	x += 12; break;
		case 'C': case 'c':	x += 13; break;
		case 'D': case 'd':	x += 14; break;
		case 'E': case 'e':	x += 15; break;
		case 'F': case 'f':	x += 16; break;
		case 'G': case 'g':	x += 17; break;
		case 'H': case 'h':	x += 18; break;
		case 'I': case 'i':	x += 19; break;
		case 'J': case 'j':	x += 20; break;
		case 'K': case 'k':	x += 21; break;
		case 'L': case 'l':	x += 22; break;
		case 'M': case 'm':	x += 23; break;
		case 'N': case 'n':	x += 24; break;
		case 'O': case 'o':	x += 25; break;
		case 'P': case 'p':	x += 26; break;
		case 'Q': case 'q':	x += 27; break;
		case 'R': case 'r':	x += 28; break;
		case 'S': case 's':	x += 29; break;
		case 'T': case 't':	x += 30; break;
		case 'U': case 'u':	x += 31; break;
		case 'V': case 'v':	x += 32; break;
		case 'W': case 'w':	x += 33; break;
		case 'X': case 'x':	x += 34; break;
		case 'Y': case 'y':	x += 35; break;
		case 'Z': case 'z':	x += 36; break;
		default:		break;
		}
	}

	return x;
}
