/*	Datenbanken mit variabler Satzlänge
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

$Header	<EFEU/$1>
*/

#ifndef EFEU_VRData_h
#define EFEU_VRData_h	1

#include <EFEU/io.h>

#define	MSG_VRDATA	"db"

typedef struct {
	uchar_t *buf;	/* Datenbuffer */
	size_t recl;	/* aktuelle Satzlänge */
	size_t size;	/* Buffergröße */
} VRData_t;

#define	VRData_recl	0
#define	VRData_size	sizeof(VRData_t)

extern size_t VRData_read (io_t *io, VRData_t *data, size_t n);
extern size_t VRData_write (io_t *io, VRData_t *data, size_t n);
extern void VRData_clear (VRData_t *data, size_t n);

#endif	/* EFEU/VRData.h */
