/*
:*:	seting up GtkWidget class
:de:	GtkWidget Klasse initialisieren

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

#include <GUI/EGtkWidget.h>
#include <EFEU/CmdPar.h>
#include <EFEU/parsedef.h>

Type_t EGtkWidgetType = STD_TYPE("GtkWidget", GtkWidget *,
	&EGtkObjectType, NULL, NULL);

void EGtkWidgetClass(GtkType gtktype, EGtkArgDef *arg, size_t narg,
	EGtkSigDef *sig, size_t nsig)
{
	Type_t *type = EGtkType(gtktype);
	AddEGtkArg(type, arg, narg);
	AddEGtkSig(type, sig, nsig);
}

/*	Widget - Variablen
*/

static EGtkArgDef arg_widget[] = {
	{ "str", "GtkWidget::name" },
	{ NULL, "GtkWidget::parent" },
	{ "int", "GtkWidget::x" },
	{ "int", "GtkWidget::y" },
	{ "int", "GtkWidget::width" },
	{ "int", "GtkWidget::height" },
	{ "bool", "GtkWidget::visible" },
	{ "bool", "GtkWidget::sensitive" },
	{ "bool", "GtkWidget::app_paintable" },
	{ "bool", "GtkWidget::can_focus" },
	{ "bool", "GtkWidget::has_focus" },
	{ "bool", "GtkWidget::has_default" },
	{ "bool", "GtkWidget::receive_default" },
	{ "bool", "GtkWidget::composite_child" },
};

/*	Widget - Signale
*/

static EGtkSigDef sig_widget[] = {
	{ "show", EGtkSigFunc_simple },
	{ "hide", EGtkSigFunc_simple },
	{ "map", EGtkSigFunc_simple },
	{ "unmap", EGtkSigFunc_simple },
	{ "realize", EGtkSigFunc_simple },
	{ "unrealize", EGtkSigFunc_simple },
	/*
	{ "draw", EGtkSigFunc_draw },
	*/
	{ "draw_focus", EGtkSigFunc_simple },
	{ "draw_default", EGtkSigFunc_simple },
	/*
	{ "size_request", EGtkSigFunc_size_request },
	{ "size_allocate", EGtkSigFunc_size_allocate },
	{ "state_changed", EGtkSigFunc_state_changed },
	{ "parent_set", EGtkSigFunc_parent_set },
	{ "style_set", EGtkSigFunc_style_set },
	{ "add_accelerator", EGtkSigFunc_add_accelerator },
	{ "remove_accelerator", EGtkSigFunc_remove_accelerator },
	*/
	{ "grab_focus", EGtkSigFunc_simple },
	/*
	{ "event", EGtkSigFunc_event },
	{ "button_press_event", EGtkSigFunc_button_press_event },
	{ "button_release_event", EGtkSigFunc_button_release_event },
	{ "motion_notofy_event", EGtkSigFunc_motion_notofy_event },
	{ "delete_event", EGtkSigFunc_delete_event },
	{ "destroy_event", EGtkSigFunc_destroy_event },
	{ "expose_event", EGtkSigFunc_expose_event },
	{ "key_press_event", EGtkSigFunc_key_press_event },
	{ "key_release_event", EGtkSigFunc_key_release_event },
	{ "enter_notify_event", EGtkSigFunc_enter_notify_event },
	{ "leave_notify_event", EGtkSigFunc_leave_notify_event },
	{ "configure_event", EGtkSigFunc_configure_event },
	{ "focus_in_event", EGtkSigFunc_focus_in_event },
	{ "focus_out_event", EGtkSigFunc_focus_out_event },
	{ "map_event", EGtkSigFunc_map_event },
	{ "unmap_event", EGtkSigFunc_unmap_event },
	{ "property_notify_event", EGtkSigFunc_property_notify_event },
	{ "selection_clear_event", EGtkSigFunc_selection_clear_event },
	{ "selection_request_event", EGtkSigFunc_selection_request_event },
	{ "selection_notify_event", EGtkSigFunc_selection_notify_event },
	{ "selection_received", EGtkSigFunc_selection_received },
	{ "selection_get", EGtkSigFunc_selection_get },
	{ "proximity_in_event", EGtkSigFunc_proximity_in_event },
	{ "proximity_out_event", EGtkSigFunc_proximity_out_event },
	{ "drag_begin", EGtkSigFunc_drag_begin },
	{ "drag_motion_mouse", EGtkSigFunc_drag_motion_mouse },
	{ "drag_leave", EGtkSigFunc_drag_leave },
	{ "drag_drop", EGtkSigFunc_drag_drop },
	{ "drag_data_get", EGtkSigFunc_drag_data_get },
	{ "drag_data_received", EGtkSigFunc_drag_data_received },
	{ "drag_data_delete", EGtkSigFunc_drag_data_delete },
	{ "drag_end", EGtkSigFunc_drag_end },
	{ "visibility_notify_event", EGtkSigFunc_visibility_notify_event },
	{ "client_event", EGtkSigFunc_client_event },
	{ "no_expose_event", EGtkSigFunc_no_expose_event },
	{ "debug_msg", EGtkSigFunc_debug_msg },
	*/
};


/*	Hilfsfunktionen
*/

static FuncDef_t fdef[] = {
	{ 0, NULL, "GtkObject GtkWidget ()", EGtkTypeCast },
};


/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der GtkWidget Klasse.
:*:
The function |$1| expands the esh-Interpreter with the GtkWidget class.
*/

void EGtkWidget_setup (void)
{
	AddType(&EGtkWidgetType);
	AddEGtkArg(&EGtkWidgetType, arg_widget, tabsize(arg_widget));
	AddEGtkSig(&EGtkWidgetType, sig_widget, tabsize(sig_widget));

	EGtkEnum_setup();
	EGtkContainer_setup();
	EGtkMisc_setup();
	EGtkEditable_setup();

	EGtkType(gtk_hseparator_get_type());
	EGtkType(gtk_vseparator_get_type());

	AddFuncDef(fdef, tabsize(fdef));
}

/*
$SeeAlso
\mref{EGtk(7)}.
*/
