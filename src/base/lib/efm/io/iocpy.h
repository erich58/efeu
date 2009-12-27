/*	IO-Kopierdefinitionen
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_IOCPY_H
#define	EFEU_IOCPY_H	1

#include <EFEU/io.h>

typedef struct {
	char *key;
	char *arg;
	unsigned flags;
	int (*func) (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
} iocpy_t;


int iocpy (io_t *in, io_t *out, iocpy_t *def, size_t dim);
char *miocpy (io_t *in, iocpy_t *def, size_t dim);
int iocpyfmt (const char *fmt, io_t *out, iocpy_t *def, size_t dim);
char *miocpyfmt (const char *fmt, iocpy_t *def, size_t dim);
io_t *cpyfilter (io_t *io, iocpy_t *def, size_t dim);

extern int iocpy_flag;	/* Flag für Sonderzeichen */
extern int iocpy_last;	/* Zuletzt gelesenes Zeichen */


/*	Standardkopierroutinen
*/

int iocpy_esc (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_mark (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_repl (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_skip (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_cskip (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_psub (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_eval (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_term (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_value (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_name (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_str (io_t *in, io_t *out, int key, const char *arg, unsigned flags);
int iocpy_xstr (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

int iocpy_usage (io_t *in, io_t *out, int key, const char *arg, unsigned flags);

#endif	/* EFEU/iocpy.h */
