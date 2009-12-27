/*	Konvertierungsprogramme
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include "konv.h"


int B80_to_wkl(int val)

{
	switch (val)
	{
	case  1:	return 11;
	case  2:	return 11;
	case  3:	return 12;
	case  4:	return 12;
	case  5:	return 12;
	case  6:	return 12;
	case  7:	return 0;
	case  8:	return 13;
	case  9:	return 13;
	case 10:	return 13;
	case 11:	return 0;
	case 12:	return 0;
	case 13:	return 0;
	case 14:	return 1;
	case 15:	return 1;
	case 16:	return 2;
	case 17:	return 2;
	case 18:	return 2;
	case 19:	return 2;
	case 20:	return 2;
	case 21:	return 2;
	case 22:	return 2;
	case 23:	return 2;
	case 24:	return 2;
	case 25:	return 2;
	case 26:	return 3;
	case 27:	return 3;
	case 28:	return 3;
	case 29:	return 3;
	case 30:	return 3;
	case 31:	return 4;
	case 32:	return 4;
	case 33:	return 4;
	case 34:	return 4;
	case 35:	return 4;
	case 36:	return 4;
	case 37:	return 4;
	case 38:	return 5;
	case 39:	return 5;
	case 40:	return 5;
	case 41:	return 6;
	case 42:	return 6;
	case 43:	return 6;
	case 44:	return 7;
	case 45:	return 8;
	case 46:	return 14;
	case 47:	return 8;
	case 48:	return 8;
	case 49:	return 8;
	case 50:	return 15;
	case 51:	return 9;
	case 52:	return 9;
	case 53:	return 9;
	case 54:	return 10;
	case 55:	return 10;
	case 56:	return 10;
	case 57:	return 10;
	case 58:	return 16;
	default:	return 17;
	}
}


int S07_to_gembez(int val)

{
	return (val >= 99) ? val - 98 : 0;
}
