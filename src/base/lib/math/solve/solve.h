/*	Nullstellenberechnung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	MATH_SOLVE_H
#define	MATH_SOLVE_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>


/*	Nullstellenberechnung
*/

#ifndef	MSG_SOLVE
#define	MSG_SOLVE	"solve"	/* Name für Fehlermeldungen */
#endif


double regfalsi (double (*f)(void *par, double x),
	void *fpar, double x0, double x1);
double bisection (int (*f)(void *par, double x),
	void *fpar, double x0, double x1);

void SetupSolve (void);	/* Parameter für Nullstellenberechnung */

extern int SolveFlag;	/* Protokollflag */
extern char *SolveFmt;	/* Ausgabeformat */
extern double SolveEps;	/* Genauigkeit */
extern int SolveStep;	/* Max. Stufenzahl */

#endif	/* MATH_SOLVE_H */
