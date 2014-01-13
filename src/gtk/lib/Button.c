/*
:*:	setting up GtkButton widgets
:de:	GtkButton Widgets initialisieren

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
#include <EFEU/stdtype.h>
#include <EFEU/CmdPar.h>

#if	HAS_GTK

#define	STR(arg)	(Val_ptr(arg) ? Val_ptr(arg) : "")

static EGtkArgDef arg_button[] = {
	{ "str", "GtkButton::label" },
};

static EGtkSigDef sig_button[] = {
	{ "pressed", EGtkSigFunc_simple },
	{ "released", EGtkSigFunc_simple },
	{ "clicked", EGtkSigFunc_simple },
	{ "enter", EGtkSigFunc_simple },
	{ "leave", EGtkSigFunc_simple },
};

static EGtkArgDef arg_toggle_button[] = {
	{ "bool", "GtkToggleButton::active" },
	{ "bool", "GtkToggleButton::draw_indicator" },
};

static EGtkSigDef sig_toggle_button[] = {
	{ "toggled", EGtkSigFunc_simple },
};

static void f_button (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_button_new_with_label (STR(arg[0]));
}

static void f_lbutton (EfiFunc *func, void *rval, void **arg)
{
	GtkWidget *button = gtk_button_new_with_label (STR(arg[0]));
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		GTK_SIGNAL_FUNC(EGtkSigFunc_simple), RefObj(Val_obj(arg[1])));
	Val_ptr(rval) = button;
}

static void f_wbutton (EfiFunc *func, void *rval, void **arg)
{
	GtkWidget *button = gtk_button_new ();
	gtk_container_add(GTK_CONTAINER(button), Val_ptr(arg[0]));
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
		GTK_SIGNAL_FUNC(EGtkSigFunc_simple), RefObj(Val_obj(arg[1])));
	Val_ptr(rval) = button;
}

static void f_toggle_button (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_toggle_button_new_with_label (STR(arg[0]));
}

static void f_check_button (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_check_button_new_with_label (STR(arg[0]));
}

static GSList *rgroup (void *arg)
{
	return Val_ptr(arg) ? gtk_radio_button_group(Val_ptr(arg)) : NULL;
}

static void f_radio_button1 (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_radio_button_new (rgroup(arg[0]));
}

static void f_radio_button2 (EfiFunc *func, void *rval, void **arg)
{
	Val_ptr(rval) = gtk_radio_button_new_with_label (rgroup(arg[0]),
		STR(arg[1]));
}

static EfiFuncDef fdef[] = {
	{ 0, NULL, "GtkButton GtkButton (GtkWidget widget, Expr_t expr)",
		f_wbutton },
	{ 0, NULL, "GtkButton GtkButton (str label, Expr_t expr)", f_lbutton },
	{ 0, NULL, "GtkButton GtkButton (str label)", f_button },
	{ 0, NULL, "GtkToggleButton GtkToggleButton (str label)",
		f_toggle_button },
	{ 0, NULL, "GtkCheckButton GtkCheckButton (str label)",
		f_check_button },
	{ 0, NULL, "GtkRadioButton GtkRadioButton (GtkRadioButton grp)",
		f_radio_button1 },
	{ 0, NULL, "GtkRadioButton GtkRadioButton"
		"(GtkRadioButton grp, str label)",
		f_radio_button2 },
};

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der GtkButton Widgets.
:*:
The function |$1| expands the esh-Interpreter with the GtkButton widgets.
*/

void EGtkButton_setup (void)
{
	EGtkWidgetClass(gtk_button_get_type(),
		arg_button, tabsize(arg_button),
		sig_button, tabsize(sig_button));
	EGtkWidgetClass(gtk_toggle_button_get_type(),
		arg_toggle_button, tabsize(arg_toggle_button),
		sig_toggle_button, tabsize(sig_toggle_button));
	EGtkWidgetClass(gtk_check_button_get_type(),
		NULL, 0, NULL, 0);
	EGtkWidgetClass(gtk_radio_button_get_type(),
		NULL, 0, NULL, 0);
	AddFuncDef(fdef, tabsize(fdef));
}

#endif

/*
$SeeAlso
\mref{EGtk(7)}.
*/
