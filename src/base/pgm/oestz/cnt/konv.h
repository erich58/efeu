/*	Konvertierungsfunktionen
	(c) 1992 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#ifndef	KONV_H
#define	KONV_H	1

#include <EFEU/efmain.h>
#include <EFEU/mdmat.h>

#define	DIM_WKL	18	/* Zahl der Wirtschaftsklassen */

int B80_to_wkl (int val);
int S07_to_gembez (int val);

#endif	/* KONV_H */
