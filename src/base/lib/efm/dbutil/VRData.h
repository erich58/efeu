/*
Datenbanken mit variabler Satzlänge

$Header	<EFEU/$1>

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
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
