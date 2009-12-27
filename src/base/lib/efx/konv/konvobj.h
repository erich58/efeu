/*	Objektkonvertierung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_KONVOBJ_H
#define	EFEU_KONVOBJ_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>


/*	Distanzen
*/

#define	D_REJECT	-1	/* Konvertierung ablehnen */
#define	D_MATCH		0	/* Übereinstimung */

#define	D_PROMOTE	01	/* Geförderte Konvertierung */
#define	D_EXPAND	02	/* Expansion eines Basisobjekts */
#define	D_KONVERT	04	/* Normale Konvertierung */

#define	D_BASE		010	/* Kompatible Basis */
#define	D_ACCEPT	020	/* Akzeptierte Übernahme */
#define	D_VAARG		040	/* Variable Argumentzahl */

#define	D_RESTRICTED	0100	/* Eingeschränkte Konvertierung */
#define	D_CREATE	0200	/* Konstruktor verwenden */
#define	D_MAXDIST	0400	/* Maximaldistanz */


/*	Funktionsargumente Konvertieren
*/

typedef struct {
	Func_t *func;
	Type_t *type;
	int dist;
} ArgKonv_t;

ArgKonv_t *GetArgKonv (const Type_t *old, const Type_t *new);
int ArgKonvDist (const Type_t *old, const Type_t *new);

Obj_t *KonvObj (const Obj_t *obj, Type_t *def);
void ArgKonv (ArgKonv_t *konv, void *tg, void *src);

Func_t *SearchFunc (VirFunc_t *tab, FuncArg_t *arg,
	size_t narg, ArgKonv_t **konv);

Func_t *GetKonvFunc(const Type_t *old, const Type_t *new);


/*	Objekte konvertieren
*/

typedef struct {
	Func_t *func;	/* Konvertierungsfunktion */
	Type_t *type;	/* Konvertierungsbasis */
	int dist;	/* Konvertierungsdistanz */
} Konv_t;

Konv_t *GetKonv (Konv_t *buf, const Type_t *otype, const Type_t *ntype);
void KonvData (Konv_t *konv, void *tg, void *src);
int KonvDist (const Type_t *old, const Type_t *new);

#endif	/* EFEU_KONVOBJ_H */
