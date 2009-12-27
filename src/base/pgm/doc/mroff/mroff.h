/*	mroff-Ausgabefilter
	(c) 1999 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5

	Version 0.4
*/

#ifndef	_EFEU_mroff_h
#define	_EFEU_mroff_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include <DocDrv.h>


typedef struct {
	DOCDRV_VAR;		/* Standardausgabevariablen */
	char *nextpar;		/* N�chster Absatz */
	unsigned copy : 1;	/* Kopiermodus */
	unsigned space : 1;	/* Leerzeichen wird ben�tigt */
	unsigned nlignore : 1;	/* N�chsten Zeilenvorschub ignorieren */
	unsigned item : 1;	/* Flag f�r item */
	stack_t *stack;		/* Stack f�r Umgebungen */
	stack_t *s_att;	/* Stack mit Attributen */
	char *att;	/* Aktuelles Attribute */
} mroff_t;

extern io_t *DocOut_mroff (io_t *io);

extern int mroff_putc (mroff_t *mr, int c);
extern int mroff_plain (mroff_t *mr, int c);
extern int mroff_protect (mroff_t *mr, int c);

extern void mroff_cbeg (mroff_t *mr, const char *pfx);
extern void mroff_cend (mroff_t *mr, int flag);

extern void mroff_string (mroff_t *mr, const char *str);
extern void mroff_newline (mroff_t *mr);
extern void mroff_rem (mroff_t *mr, const char *rem);
extern int mroff_cmd (mroff_t *mr, va_list list);
extern int mroff_env (mroff_t *mr, int flag, va_list list);
extern void mroff_cmdline (mroff_t *mr, const char *name);
extern void mroff_cmdend (mroff_t *mr, const char *name);

#endif	/* EFEU/mroff.h */
