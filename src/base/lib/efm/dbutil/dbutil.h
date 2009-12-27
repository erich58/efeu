/*	Datenbankhilfsprogramme
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

$Header	<EFEU/$1>
*/

#ifndef EFEU_DBUTIL_H
#define EFEU_DBUTIL_H	1

#include <EFEU/ftools.h>

#define	MSG_DB		"db"

typedef size_t (*db_read_t) (void *data, size_t size, size_t nitems, FILE *file);
typedef size_t (*db_write_t) (const void *data, size_t size, size_t nitems, FILE *file);

typedef struct {
	FILE *file;
	db_read_t read;
	db_write_t write;
	void *buf;
	size_t reclen;
} DBFILE;


DBFILE *db_open (const char *name, const char *mode, size_t size);
DBFILE *db_xopen (const char *name, const char *mode, size_t size,
	db_read_t read, db_write_t write);

void db_close (DBFILE *db);
void *db_get (DBFILE *db);
void *db_last (DBFILE *db);
int db_put (const void *data, DBFILE *db);
int db_uniq (const void *data, DBFILE *db);

size_t db_read (void *data, size_t dim, DBFILE *db);
size_t db_write (const void *data, size_t dim, DBFILE *db);


/*	Konvertierungshilfsprogramme
*/

int db_isblank (const uchar_t *buf, int pos, int len);
int db_iskey (const uchar_t *buf, int key, int pos, int len);
unsigned db_xcval (const uchar_t *buf, int pos, int len);
unsigned db_cval (const uchar_t *buf, int pos, int len);
unsigned db_pval (const uchar_t *buf, int pos, int len);
unsigned db_xval (const uchar_t *buf, int pos, int len);
unsigned db_bcdval (const uchar_t *buf, int pos, int len);
unsigned db_a37l (const uchar_t *buf, int pos, int len);
unsigned db_char (const uchar_t *buf, int pos, int len);
char *db_str (const uchar_t *buf, int pos, int len);
double db_double (const uchar_t *buf, int pos, int len);


/*	Dateien sortieren
*/

extern char *dbsort_tmpdir;
extern char *dbsort_prefix;
extern int dbsort_debug;

void db_sort (DBFILE *in, DBFILE *out, size_t blksize, comp_t comp, int uniq);
void db_fsort (const char *iname, const char *oname, size_t elsize,
	db_read_t read, db_write_t write, size_t blksize, comp_t comp, int uniq);

/*	Zahlendarstellung zur Basis 37
*/

#define	DIG37_SPACE	0
#define	DIG37_0		1
#define	DIG37_1		2
#define	DIG37_2		3
#define	DIG37_3		4
#define	DIG37_4		5
#define	DIG37_5		6
#define	DIG37_6		7
#define	DIG37_7		8
#define	DIG37_8		9
#define	DIG37_9		10
#define	DIG37_A		11
#define	DIG37_B		12
#define	DIG37_C		13
#define	DIG37_D		14
#define	DIG37_E		15
#define	DIG37_F		16
#define	DIG37_G		17
#define	DIG37_H		18
#define	DIG37_I		19
#define	DIG37_J		20
#define	DIG37_K		21
#define	DIG37_L		22
#define	DIG37_M		23
#define	DIG37_N		24
#define	DIG37_O		25
#define	DIG37_P		26
#define	DIG37_Q		27
#define	DIG37_R		28
#define	DIG37_S		29
#define	DIG37_T		30
#define	DIG37_U		31
#define	DIG37_V		32
#define	DIG37_W		33
#define	DIG37_X		34
#define	DIG37_Y		35
#define	DIG37_Z		36

unsigned a37l (const char *s);
char *l37a (unsigned x);
int ebcdic2ascii (int x);

#endif	/* EFEU/dbutil.h */
