/*	Generierung einer Dezilverteilung
	(c) 1998 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 3.1
*/

#ifndef	DEZILE_H
#define	DEZILE_H	1

#include <EFEU/mdmat.h>
#include <EFEU/Info.h>
#include <Math/pnom.h>


typedef struct {
	double x;
	char *name;
	char *label;
} LimDef_t;

typedef struct {
	char *name;
	char *label;
	LimDef_t *tab;
	size_t dim;
} ModeDef_t;

ModeDef_t *GetMode (const char *name);
void ModeInfo (InfoNode_t *info, const char *name, const char *label);

pnom_t *intpol (size_t n, double *x, double *y);

#endif	/* DEZILE_H */
