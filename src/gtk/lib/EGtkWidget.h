/*
:*:	Gtk+ widget definitions
:de:	Gtk+ Widget Definitionen

$Header <GUI/EGtkWidget.h>
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

#ifndef	GUI_EGtkWidget_h
#define	GUI_EGtkWidget_h	1

#include <GUI/EGtk.h>
#include <EFEU/stdtype.h>

extern Type_t EGtkWidgetType;
extern Type_t EGtkDataType;

extern GtkWidget *Obj2GtkWidget (Obj_t *obj);
extern void AddEGtkWidgetType (Type_t *type);

extern void EGtkEnum_setup (void);
extern void EGtkWidget_setup (void);
extern void EGtkContainer_setup (void);
extern void EGtkList_setup (void);
extern void EGtkBox_setup (void);
extern void EGtkButton_setup (void);
extern void EGtkEditable_setup (void);
extern void EGtkMisc_setup (void);
extern void EGtkWindow_setup (void);

extern Obj_t *EGtkArg2Obj (GtkArg *arg);
extern Obj_t *EGtkArg2Lval (GtkObject *obj, const char *name);

typedef struct {
	OBJECT_VAR;
	GtkObject *obj;
	GtkArg arg;
} EGtkArg;

extern Obj_t *GetEGtkArg (const Var_t *st, const Obj_t *obj);
extern Obj_t *EGtkObjectArg (GtkObject *obj, Type_t *type, const char *name);

typedef struct {
	char *type;
	char *name;
} EGtkArgDef;

extern void AddEGtkArg(Type_t *type, EGtkArgDef *def, size_t dim);
extern void EGtkWidgetClass(GtkType type, EGtkArgDef *arg, size_t narg,
	EGtkSigDef *sig, size_t nsig);

extern Obj_t *EGtkLval_alloc (Type_t *type, va_list list);
extern void EGtkLval_free (Obj_t *obj);
extern char *EGtkLval_ident (Obj_t *obj);

#define	EGTK_LVAL(name, update, sync)	\
Lval_t name = { EGtkLval_alloc, EGtkLval_free, update, sync, EGtkLval_ident }

/*
$SeeAlso
\mref{SetupEGtk(3)}.
*/

#endif	/* GUI/EGtkWidget.h */
