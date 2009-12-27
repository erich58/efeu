/*	Mathematische Datenmatrixfunktionen
	(c) 1997 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 1.0
*/

#ifndef	Math_mdmath_h
#define	Math_mdmath_h	1

#include <EFEU/mdmat.h>

#define	MSG_MDMATH	"mdmath"

mdmat_t *mdinv (mdmat_t *base);

void SetupMdMath (void);

#endif	/* Math_mdmath_h */
