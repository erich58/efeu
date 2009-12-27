/*	Konfiguration der Initialisierung
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_CMDCONFIG_H
#define	EFEU_CMDCONFIG_H	1

#include <EFEU/cmdsetup.h>

#define	MAKE_ASSIGN_OP	1

#define	CEXPR(name, expr)	\
static void name (Func_t *func, void *rval, void **arg) \
{ expr; }

#define	LEXPR(name, expr)	\
static void name (Func_t *func, void *rval, void **arg) \
{ expr; Val_ptr(rval) = arg[0]; }

#endif	/* EFEU_CMDCONFIG_H */
