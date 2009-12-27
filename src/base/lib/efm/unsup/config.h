/*	Makrodeinitionen für Programmkomapatiblität
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_CONFIG_H
#define	EFEU_CONFIG_H	1

/*	Kompilerkennungen

ISC	GNU C-Kompiler für ISC UNIX 386/ix
ALPHA	GNU C-Kompiler für linux auf ALPHA
LINUX	GNU C-Kompiler für linux
QC	Quick C-Kompiler für MSDOS, OS/2
GO32	GNU-C Portierung für MSDOS
AIX	C-Kompiler für AIX
*/

#if	GO32 || ALPHA || LINUX || ISC || AIX || GC
#error	Kompilerkennung darf nicht vordefiniert sein
#endif

#if	__GO32__
#define	GO32	1
#elif	__alpha__
#define	ALPHA	1
#elif	__linux__
#define	LINUX	1
#elif	i386 || __i386__
#define	ISC	1
#elif	_AIX
#define	AIX	1
#elif	_QC
#define	QC	1
#else
#error	Unbekannter Kompiler
#endif


/*	Systemspezifische Testmakros
	============================

HAS_SHORTSEG	Falls 1, sind Speichersegmente mit 64k begrenzt.
HAS_SHORTINT	Falls 1, ist eine Integervariable äquivalent zu short.
REVBYTEORDER	Falls 1, werden niedrigsignifikante Bytes vor 	
		höhersignifikanten Bytes abgespeichert.
MSDOS		Falls 1, wird unter MSDOS kompiliert
*/

#if	QC
#define	HAS_SHORTSEG	1
#define	HAS_SHORTINT	1
#else
#define	HAS_SHORTSEG	0
#define	HAS_SHORTINT	0
#endif

#if	AIX
#define	REVBYTEORDER	0
#else
#define	REVBYTEORDER	1
#endif

#if	QC || GO32

#ifndef	MSDOS
#define	MSDOS	1
#endif

#else

#define	MSDOS	0

#endif


/*	Standarddefinitionen
	====================

Die Typdeklaration "size_t", sowie die wichtigsten Makros und
Funktionsdeklarationen der ANSI-Headerdateien <stddef.h> und
<stdlib.h> werden verfügbar gemacht. Die Headerdateien <stdio.h>
und <string.h> werden eingebunden.

Macros:

	NULL, SEEK_SET, SEEK_CUR, SEEK_END,
*/


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/*	Standardmakros
*/

#ifndef	NULL
#define	NULL	0
#endif

#ifndef	SEEK_SET
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2
#endif

/*	Unsigned - Datentypen
*/

#if	ISC || ALPHA || LINUX || QC
typedef unsigned char uchar_t;
typedef unsigned short ushort_t;
typedef unsigned long ulong_t;
#endif

/*	Allgemeine Hilfsmakros und Hilfsfunktionen
*/

#ifndef	tabsize
#define	tabsize(x)	(sizeof(x) / sizeof(x[0]))
#endif

#ifndef	sizealign
#define	sizealign(x, y)	((y) * (((x) + (y) - 1) / (y)))
#endif

#endif	/* EFEU/config.h */
