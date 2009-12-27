/*	Parametersubstitution
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_PARSUB_H
#define	EFEU_PARSUB_H	1

#include <EFEU/iocpy.h>

typedef int (*iocopy_t) (io_t *in, io_t *out, void *par);

typedef struct {
	int key;
	iocopy_t copy;
	void *par;
} copydef_t;

int iocopystr (const char *fmt, io_t *out, iocopy_t copy, void *arg);
char *miocopy (io_t *in, iocopy_t copy, void *arg);
char *miocopystr (const char *fmt, iocopy_t copy, void *arg);

char *parsub (const char *fmt);
void psubfunc (int key, iocopy_t copy, void *par);
int io_pcopy (io_t *in, io_t *out, void *par);
char *mpcopy (io_t *in);
int io_psub (io_t *io, const char *fmt);

extern int psub_key;

char *reg_get (int n);
char *reg_set (int n, char *arg);
char *reg_cpy (int n, const char *arg);
char *reg_str (int n, const char *arg);
char *reg_fmt (int n, const char *fmt, ...);
void reg_clear (void);

#endif	/* EFEU/parsub.h */
