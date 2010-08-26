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

#ifndef	DocDrv_h
#define	DocDrv_h	1

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/vecbuf.h>
#include <EFEU/object.h>
#include <efeudoc/DocCtrl.h>
#include <efeudoc/DocSym.h>


#define	DOCDRV_VAR		\
	const char *name;	\
	unsigned submode : 22;	\
	unsigned skip : 1;	\
	unsigned hflag : 1;	\
	char last;		\
	IO *out;		\
	EfiVarTab *vartab;	\
	DocSym *symtab;	\
	int (*put) (void *drv, int c); \
	int (*putucs) (void *drv, int32_t c); \
	void (*sym) (void *drv, const char *symbol); \
	void (*rem) (void *drv, const char *remark); \
	void (*hdr) (void *drv, int mode); \
	int (*cmd) (void *drv, va_list list); \
	int (*env) (void *drv, int mode, va_list list)

typedef struct {
	DOCDRV_VAR;
} DocDrv;

extern void *DocDrv_alloc (const char *name, size_t size);
extern void DocDrv_free (void *drv);
extern void DocDrv_var (void *drv, EfiType *type, const char *name, void *ptr);
extern void DocDrv_eval (void *ptr, const char *name);
extern IO *DocDrv_io (void *drv);
extern int DocDrv_plain (void *drv, int c);

#endif	/* DocDrv.h */
