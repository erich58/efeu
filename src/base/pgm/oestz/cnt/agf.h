/*	Haeuser und Wohnungszaehlung
	(c) 1992 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6

	Version 1.0
*/

#ifndef	VZ_H
#define	VZ_H	1

#include <EFEU/efmain.h>
#include <EFEU/mdcount.h>
#include "konv.h"


typedef struct {
	char *name;
	char *desc;
	char *file;
	size_t size;
	void (*mksl) (xtab_t *tab);
	void (*cval) (ushort_t *data);
	int stdcount;
} DBDEF;


DBDEF *dbdef (const char *name);

typedef struct SLDEF_S {
	char *name;
	char *desc;
	size_t sl_dim;
	char *idxfmt;
	int (*classify) (struct SLDEF_S *cdef, ushort_t *data);
	int num;
} SLDEF;


void sl_asza81 (xtab_t *tab);
void sl_aszb81 (xtab_t *tab);
void sl_asza91 (xtab_t *tab);
void sl_aszb91 (xtab_t *tab);
void sl_hwz81 (xtab_t *tab);
void sl_hwz91 (xtab_t *tab);
void sl_vz81 (xtab_t *tab);
void sl_pendler81 (xtab_t *tab);
void sl_wohnbau (xtab_t *tab);
void sl_wbneu (xtab_t *tab);

void c_asza81 (ushort_t *data);
void c_aszb81 (ushort_t *data);
void c_asza91 (ushort_t *data);
void c_aszb91 (ushort_t *data);
void c_hwz81 (ushort_t *data);
void c_hwz91 (ushort_t *data);
void c_vz81 (ushort_t *data);
void c_pendler81 (ushort_t *data);
void c_wohnbau (ushort_t *data);
void c_wbneu (ushort_t *data);

void open_db (DBDEF *def);
ushort_t *get_db (void);
void close_db (void);

void dblist (void);

extern xtab_t sldef;
extern ushort_t wohnungen;
extern ushort_t personen;
extern ushort_t arbst;
extern ushort_t ausl;
extern int aufwand;
extern int nfl;

void SetupVZ (void);

#endif	/* VZ_H */
