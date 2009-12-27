/*	SGML-Ausgabefilter
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#ifndef	_EFEU_SGML_h
#define	_EFEU_SGML_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <DocDrv.h>


typedef struct {
	DOCDRV_VAR;	/* Standardausgabevariablen */
	int class;	/* Dokumentklasse */
	int copy;	/* Kopiermodus */
	char *nextpar;	/* Absatzkennung */
	stack_t *s_att;	/* Stack mit Attributen */
	char *att;	/* Aktuelles Attribute */
} SGML_t;

extern io_t *DocOut_sgml (io_t *io);

extern int SGML_putc (SGML_t *sgml, int c);
extern int SGML_plain (SGML_t *sgml, int c);
extern void SGML_puts (SGML_t *sgml, const char *str);
extern void SGML_newline (SGML_t *sgml, int n);
extern void SGML_rem (SGML_t *sgml, const char *rem);
extern void SGML_sym (SGML_t *sgml, const char *sym);
extern int SGML_cmd (SGML_t *sgml, va_list list);
extern int SGML_env (SGML_t *sgml, int flag, va_list list);

#endif	/* EFEU/SGML.h */
