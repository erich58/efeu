/*	Zufallszahlengenerator
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#ifndef	EFEU_Random_h
#define	EFEU_Random_h	1

#include <EFEU/refdata.h>

#if	AIX

extern long random (void);
extern void srandom (int sval);
extern void *initstate (unsigned sval, void *state, int dim);
extern void *setstate (void *state);

#endif

#define	DRAND_KOEF	(1. / 2147483648.)	/* 2^-31 */

typedef struct {
	REFVAR;
	void *state;
	int size;
} Random_t;

extern reftype_t Random_reftype;

Random_t *Random (int sval, int dim);
void SeedRandom (Random_t *rd, int sval);
long LongRandom (Random_t *rd);
double UniformRandom (Random_t *rd);
double LinearRandom (Random_t *rd, double z);
double NormalRandom (Random_t *rd);
int PoissonRandom (Random_t *rd, double mw);
int RoundRandom (Random_t *rd, double val);
size_t RandomIndex (Random_t *rd, size_t dim);

void SetupRandom (void);

#endif	/* EFEU_Random_h */
