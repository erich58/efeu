/*	Nullstellenberechnung
	(c) 1994 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/

#ifndef	MATH_SOLVE_H
#define	MATH_SOLVE_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>


/*	Nullstellenberechnung
*/

#ifndef	MSG_SOLVE
#define	MSG_SOLVE	"solve"	/* Name f�r Fehlermeldungen */
#endif


double regfalsi (double (*f)(void *par, double x),
	void *fpar, double x0, double x1);
double bisection (int (*f)(void *par, double x),
	void *fpar, double x0, double x1);

void SetupSolve (void);	/* Parameter f�r Nullstellenberechnung */

extern int SolveFlag;	/* Protokollflag */
extern char *SolveFmt;	/* Ausgabeformat */
extern double SolveEps;	/* Genauigkeit */
extern int SolveStep;	/* Max. Stufenzahl */

#endif	/* MATH_SOLVE_H */
