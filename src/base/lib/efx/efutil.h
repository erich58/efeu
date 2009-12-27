/*	Hilfsprogramme
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_EFUTIL_H
#define	EFEU_EFUTIL_H	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/ftools.h>
#include <EFEU/dbutil.h>

#ifndef	MSG_EFUTIL
#define	MSG_EFUTIL	"efutil"	/* Name für Fehlermeldungen */
#endif

/*	Interpreterprogramme
*/

int EshConfig (int *narg, char **arg);

/*	String/Zeile laden
*/

int io_egetc (io_t *io, const char *delim);
char *io_lgets (io_t *io, const char *endkey);
int iocpy_brace (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
char *getstring (io_t *io);


/*	Zufallszahlengenereator
*/

double drand48 (void);
long lrand48 (void);
void srand48 (long seedval);

void SetupRand48 (void);
void SetupUtil (void);

/*	Rundungshilfsprogramm
*/

void roundvec(double *x, size_t dim, double val);


/*	Hilfsprogramme zur TeX - Ausgabe
*/

int TeXputs(const char *str, io_t *io);
int TeXquote(const char *str, io_t *io);
int TeXputc(int c, io_t *io);

#endif	/* EFEU_EFUTIL_H */
