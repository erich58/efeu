/*	Hauptdefinitionen für efmain Programmbibliothek
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_EFMAIN_H
#define	EFEU_EFMAIN_H	1

#include <EFEU/types.h>
#include <EFEU/msgtab.h>


#define	MSG_EFMAIN	"efmain"


/*	Standardmakros und Funktionstypedefinitionen
*/

#ifndef	min
#define	min(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef	max
#define	max(a, b)	((a) > (b) ? (a) : (b))
#endif

#ifndef	tabsize
#define	tabsize(x)	(sizeof(x) / sizeof(x[0]))
#endif


/*	Speicherplatzverwaltung
*/

#include <EFEU/memalloc.h>

#define	ALLOC(dim, tp)	(tp *) memalloc((size_t) (dim) * sizeof(tp))
#define	FREE(ptr)	memfree((void *) (ptr))

#include <EFEU/strbuf.h>


/*	Suchtabellen
*/

typedef struct {
	comp_t comp;	/* Vergleichsfunktion */
	void **tab;	/* Tabelle mit Pointer */
	size_t dim;	/* Tabellendimension */
	size_t tsize;	/* Tabellengröße */
	size_t bsize;	/* Blockgröße */
} xtab_t;

#define	XS_DATA(blk, comp)	{ comp, NULL, 0, 0, blk }

#define	XTAB(name, blk, comp)	xtab_t name = XS_DATA(blk, comp)

#define	XS_ENTER	0	/* Eintrag abfragen/ergänzen */
#define	XS_REPLACE	1	/* Eintrag ersetzen/ergänzen */
#define	XS_FIND		2	/* Eintrag abfragen */
#define	XS_DELETE	3	/* Eintrag aus Tabelle löschen */

xtab_t *xcreate (size_t bsize, comp_t comp);
void *xsearch (xtab_t *tab, void *key, int flag);
int xwalk (xtab_t *tab, visit_t visit);
void xdestroy (xtab_t *tab, clean_t visit);
void xappend (xtab_t *tab, void *base, size_t nel, size_t width, int flag);

#define	XAPPEND(t,x,f)	xappend(t, x, tabsize(x), sizeof(x[0]), f)


/*	Standardsuchtabellen
*/

typedef struct { int num; } nkey_t;
typedef struct { const char *name; } skey_t;

extern nkey_t nkey_buf;	/* Globaler Buffer für numerischen Schlüssel */
extern skey_t skey_buf;	/* Globaler Buffer für Stringschlüssel */

int nkey_cmp (const void *x1, const void *x2);
int skey_cmp (const void *x1, const void *x2);

#define	nkey_find(tab, x)	xsearch(tab, (nkey_buf.num = (x), &nkey_buf), XS_FIND)
#define	skey_find(tab, x)	xsearch(tab, (skey_buf.name = (x), &skey_buf), XS_FIND)
#define	nkey_del(tab, x)	xsearch(tab, (nkey_buf.num = (x), &nkey_buf), XS_DELETE)
#define	skey_del(tab, x)	xsearch(tab, (skey_buf.name = (x), &skey_buf), XS_DELETE)

void *nkey_get (xtab_t *tab, int num, const void *ptr);
void *skey_get (xtab_t *tab, const char *name, const void *def);
void *nkey_set (xtab_t *tab, int num, const void *ptr);
void *skey_set (xtab_t *tab, const char *name, const void *ptr);


/*	Initialisieren / Aufräumen
*/

#include <EFEU/procenv.h>

void libinit (const char *name);
void atlibexit (void (*func)(void));
void libexit (int stat);


void skiparg (int *narg, char **arg, int n);
char *argval (const char *arg);


/*	Stringhilfsfunktionen
*/

#include <EFEU/mstring.h>

size_t strsplit (const char *str, const char *delim, char ***ptr);
char *nextstr (char **ptr);
int xstrcmp(const char *a, const char *b);

int mscanf (const char *str, const char *fmt, ...);

char *parsub (const char *fmt);
void streval (const char *cmd);


/*	Datenfiles öffnen
*/

FILE *fileopen (const char *name, const char *mode);
FILE *tempopen (void);
int fileclose (FILE *file);
char *fileident (FILE *file);


/*	Mustervergleich
*/

int listcmp (const char *list, int c);
int patcmp (const char *pat, const char *str, char **ptr);
int patselect (const char *name, char **list, size_t dim);


/*	Indexvergleich
*/

typedef struct idxcmp_s {
	struct idxcmp_s *next;	/* Für Verkettungen */
	char *pattern;		/* Musterkennung */
	int flag;		/* Negationsflag */
	size_t minval;		/* Minimalwert */
	size_t maxval;		/* Maximalwert */
	int (*cmp) (struct idxcmp_s *t, const char *s, size_t n);
} idxcmp_t;

idxcmp_t *new_idxcmp (const char *def, size_t dim);
void del_idxcmp (idxcmp_t *ic);
idxcmp_t *idxcmplist (char **list, size_t ldim, size_t dim);
int idxcmp (idxcmp_t *ic, const char *name, size_t idx);

#endif	/* EFEU_EFMAIN_H */
