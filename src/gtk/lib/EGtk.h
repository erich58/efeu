/*
:*:	Gtk+ interface for EFEU-Interpreter
:de:	Gtk+ Interface für EFEU-Interpreter

$Header <GUI/EGtk.h>
$Copyright (C) 2001 Erich Frühstück
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

#ifndef	GUI_EGtk_h
#define	GUI_EGtk_h	1

#include <EFEU/object.h>
#include <gtk/gtk.h>

extern void SetupEGtk (void);
extern void EGtkRes (const char *str);
extern void EGtkInit (void);
extern void EGtkMain (void);
extern void EGtkObject_setup (void);

extern Type_t EGtkObjectType;
extern Type_t *EGtkType (GtkType gtktype);
extern Obj_t *EGtkObject (GtkObject *obj);

extern void EGtkTypeCast (Func_t *func, void *rval, void **arg);

extern ObjList_t *EGtkArgList (ObjList_t *base, GtkType type);
extern void EGtkArgInfo (io_t *io, GtkType type);

extern void EGtkSig_var (const char *name, Type_t *type, void *data);
extern void EGtkSig_start (GtkObject **ptr);
extern void EGtkSig_end (Obj_t *expr);

extern void EGtkSigFunc_simple (GtkObject *obj, Obj_t *expr);
extern void EGtkSigFunc_child (GtkObject *obj, GtkObject *child, Obj_t *expr);

typedef struct {
	char *name;
	void *func;
} EGtkSigDef;

extern void AddEGtkSig (Type_t *type, EGtkSigDef *def, size_t dim);

/*
$SeeAlso
\mref{SetupEGtk(3)}.
*/

#endif	/* GUI/EGtk.h */
