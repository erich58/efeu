/*
Dokumenttreiberstruktur

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	_DocDrv_h
#define	_DocDrv_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include "DocCtrl.h"
#include "DocSym.h"


typedef struct DocDrv_s DocDrv_t;

typedef void (*DocDrvSym_t) (DocDrv_t *drv, const char *symbol);
typedef void (*DocDrvRem_t) (DocDrv_t *drv, const char *remark);
typedef void (*DocDrvHdr_t) (DocDrv_t *drv, int mode);
typedef int (*DocDrvCmd_t) (DocDrv_t *drv, va_list list);
typedef int (*DocDrvEnv_t) (DocDrv_t *drv, int mode, va_list list);
typedef int (*DocDrvPut_t) (DocDrv_t *drv, int c);

#define	DOCDRV_VAR		\
	const char *name;	\
	unsigned submode : 22;	\
	unsigned skip : 1;	\
	unsigned hflag : 1;	\
	char last;		\
	io_t *out;		\
	VarTab_t *vartab;	\
	vecbuf_t varbuf;	\
	DocSym_t *symtab;	\
	DocDrvPut_t put;	\
	DocDrvSym_t sym;	\
	DocDrvRem_t rem;	\
	DocDrvHdr_t hdr;	\
	DocDrvCmd_t cmd;	\
	DocDrvEnv_t env		\

struct DocDrv_s {
	DOCDRV_VAR;
};

extern void *DocDrv_alloc (const char *name, size_t size);
extern void DocDrv_free (void *drv);
extern void DocDrv_var (void *drv, Type_t *type, const char *name, void *ptr);
extern void DocDrv_eval (void *ptr, const char *name);
extern io_t *DocDrv_io (void *drv);
extern int DocDrv_plain (DocDrv_t *drv, int c);

#endif	/* DocDrv.h */
