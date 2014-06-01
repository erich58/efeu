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

#if	HAS_GTK

extern EfiType EGtkWidgetType;
extern EfiType EGtkDataType;

extern GtkWidget *Obj2GtkWidget (EfiObj *obj);
extern void AddEGtkWidgetType (EfiType *type);

extern void EGtkEnum_setup (void);
extern void EGtkWidget_setup (void);
extern void EGtkContainer_setup (void);
extern void EGtkList_setup (void);
extern void EGtkBox_setup (void);
extern void EGtkButton_setup (void);
extern void EGtkEditable_setup (void);
extern void EGtkMisc_setup (void);
extern void EGtkWindow_setup (void);

extern EfiObj *EGtkArg2Obj (GtkArg *arg);
extern EfiObj *EGtkArg2Lval (GtkObject *obj, const char *name);

typedef struct {
	OBJECT_VAR;
	GtkObject *obj;
	GtkArg arg;
} EGtkArg;

extern EfiObj *GetEGtkArg (const EfiStruct *st, const EfiObj *obj);
extern EfiObj *EGtkObjectArg (GtkObject *obj, EfiType *type, const char *name);

typedef struct {
	char *type;
	char *name;
} EGtkArgDef;

extern void AddEGtkArg(EfiType *type, EGtkArgDef *def, size_t dim);
extern void EGtkWidgetClass(GtkType type, EGtkArgDef *arg, size_t narg,
	EGtkSigDef *sig, size_t nsig);

extern EfiObj *EGtkLval_alloc (const EfiType *type, va_list list);
extern void EGtkLval_free (EfiObj *obj);
extern char *EGtkLval_ident (const EfiObj *obj);

#define	EGTK_LVAL(name, label, update, sync)	\
EfiLval name = { label, NULL, \
	EGtkLval_alloc, EGtkLval_free, update, sync, \
	NULL, EGtkLval_ident }

#endif

/*
$SeeAlso
\mref{SetupEGtk(3)}.
*/

#endif	/* GUI/EGtkWidget.h */
