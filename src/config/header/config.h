/*	EFEU/config.h: Basiskopfdatei für EFEU-Programme
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
	Version 0.8
*/

#ifndef _EFEU_config_h
#define _EFEU_config_h	1

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef	NULL
#define	NULL	(void *) 0
#endif

@eval typedef size_t unsigned long
@eval typedef uchar_t unsigned char
@eval typedef ushort_t unsigned short
@eval typedef ulong_t unsigned long

@eval byteorder

#endif	/* EFEU/config.h */
