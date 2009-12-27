/*	Datenbanken mit variabler Satzl�nge
	(c) 1999 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5

$Header	<EFEU/$1>
*/

#ifndef EFEU_VRData_h
#define EFEU_VRData_h	1

#include <EFEU/io.h>

#define	MSG_VRDATA	"db"

typedef struct {
	uchar_t *buf;	/* Datenbuffer */
	size_t recl;	/* aktuelle Satzl�nge */
	size_t size;	/* Buffergr��e */
} VRData_t;

#define	VRData_recl	0
#define	VRData_size	sizeof(VRData_t)

extern size_t VRData_read (io_t *io, VRData_t *data, size_t n);
extern size_t VRData_write (io_t *io, VRData_t *data, size_t n);
extern void VRData_clear (VRData_t *data, size_t n);

#endif	/* EFEU/VRData.h */
