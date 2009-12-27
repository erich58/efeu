/*	Zufallszahlengenerator
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_Rand48_h
#define	EFEU_Rand48_h	1

#include <EFEU/refdata.h>

/*	Hilfsfunktionen
*/

extern double drand48 (void);
extern long lrand48 (void);
extern void srand48 (long seedval);

extern double erand48 (unsigned short param[3]);
extern long nrand48 (unsigned short param[3]);


/*	Zufallszahlengenerator
*/

typedef struct {
	REFVAR;
	unsigned short param[3];
} Rand48_t;

extern reftype_t Rand48_reftype;

Rand48_t *NewRand48 (int sval);
void DelRand48 (Rand48_t *rd);
void SeedRand48 (Rand48_t *rd, int sval);
double Rand48 (Rand48_t *rd);
double NormalRand48 (Rand48_t *rd, int mode);
int Rand48Round (Rand48_t *rd, double val);


/*	Schnittstelle zu Interpreter
*/

void SetupRand48 (void);

#endif	/* EFEU_Rand48_h */
