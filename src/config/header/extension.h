/*	EFEU/extension.h: Systemabh�ngige Erweiterungen
	(c) 1999 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
	Version 0.8
*/

#ifndef _EFEU_extension_h
#define _EFEU_extension_h	1

#include <EFEU/config.h>

@eval func HAS_ISATTY 'isatty(0)'
@eval -i sys/dir.h func HAS_GETPWNAM 'getpwnam(NULL)'

#endif	/* EFEU/extension.h */
