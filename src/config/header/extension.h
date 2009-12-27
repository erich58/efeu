/*	EFEU/extension.h: Systemabhängige Erweiterungen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
	Version 0.8
*/

#ifndef _EFEU_extension_h
#define _EFEU_extension_h	1

#include <EFEU/config.h>

@eval func HAS_ISATTY 'isatty(0)'
@eval -i sys/dir.h func HAS_GETPWNAM 'getpwnam(NULL)'

#endif	/* EFEU/extension.h */
