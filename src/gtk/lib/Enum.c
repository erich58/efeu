/*
:*:	setting up Gtk+ enum types
:de:	Gtk+ Aufzählungstypen generieren

$Copyright (C) 2002 Erich Frühstück
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

#define	GTK_1_2	0

/* Arrow types */
static EnumTypeDef arrow[] = {
	{ "up", GTK_ARROW_UP },
	{ "down", GTK_ARROW_DOWN },
	{ "left", GTK_ARROW_LEFT },
	{ "right", GTK_ARROW_RIGHT },
};	/* GtkArrowType */

/* Attach options (for tables) */
static EnumTypeDef attach[] = {
	{ "expand", GTK_EXPAND },
	{ "shrink", GTK_SHRINK },
	{ "fill", GTK_FILL },
};	/* GtkAttachOptions */

#if	GTK_1_2
/* Button box styles */
static EnumTypeDef buttonbox[] =  {
	{ "default", GTK_BUTTONBOX_DEFAULT_STYLE },
	{ "spread", GTK_BUTTONBOX_SPREAD },
	{ "edge", GTK_BUTTONBOX_EDGE },
	{ "start", GTK_BUTTONBOX_START },
	{ "end", GTK_BUTTONBOX_END },
};	/* GtkButtonBoxStyle */

/* Curve types */
static EnumTypeDef curve[] = {
	{ "linear", GTK_CURVE_TYPE_LINEAR },
	{ "spline", GTK_CURVE_TYPE_SPLINE },
	{ "free", GTK_CURVE_TYPE_FREE },
};	/* GtkCurveType */
#endif
 
/* Focus movement types */
static EnumTypeDef direction[] = {
	{ "forward", GTK_DIR_TAB_FORWARD },
	{ "backward", GTK_DIR_TAB_BACKWARD },
	{ "up", GTK_DIR_UP },
	{ "down", GTK_DIR_DOWN },
	{ "left", GTK_DIR_LEFT },
	{ "right", GTK_DIR_RIGHT },
};	/* GtkDirectionType */

/* justification for label and maybe other widgets (text?) */
static EnumTypeDef justification[] = {
	{ "left", GTK_JUSTIFY_LEFT },
	{ "right", GTK_JUSTIFY_RIGHT },
	{ "center", GTK_JUSTIFY_CENTER },
	{ "fill", GTK_JUSTIFY_FILL },
};	/* GtkJustification */

#if	GTK_1_2

/* GtkPatternSpec match types */
static EnumTypeDef match[] = {
	{ "all", GTK_MATCH_ALL },
	{ "all_tail", GTK_MATCH_ALL_TAIL },
	{ "head", GTK_MATCH_HEAD },
	{ "tail", GTK_MATCH_TAIL },
	{ "exact", GTK_MATCH_EXACT },
	{ "last", GTK_MATCH_LAST },
};	/* GtkMatchType */

/* Menu keyboard movement types */
static EnumTypeDef menu_dir[] = {
	{ "parent", GTK_MENU_DIR_PARENT },
	{ "child", GTK_MENU_DIR_CHILD },
	{ "next", GTK_MENU_DIR_NEXT },
	{ "prev", GTK_MENU_DIR_PREV },
};	/* GtkMenuDirectionType */
#endif

static EnumTypeDef menu_factory[] = {
	{ "menu", GTK_MENU_FACTORY_MENU },
	{ "menu_bar", GTK_MENU_FACTORY_MENU_BAR },
	{ "option_menu", GTK_MENU_FACTORY_OPTION_MENU },
};	/* GtkMenuFactoryType */

static EnumTypeDef metric[] = {
	{ "pixels", GTK_PIXELS },
	{ "inch", GTK_INCHES },
	{ "cm", GTK_CENTIMETERS },
};	/* GtkMetricType */

/* Orientation for toolbars, etc. */
static EnumTypeDef orientation[] = {
	{ "horizontal", GTK_ORIENTATION_HORIZONTAL },
	{ "vertical", GTK_ORIENTATION_VERTICAL },
};	/* GtkOrientation */

#if	GTK_1_2
/* Placement type for scrolled window */
static EnumTypeDef corner[] = {
	{ "top_left", GTK_CORNER_TOP_LEFT },
	{ "bottom_left", GTK_CORNER_BOTTOM_LEFT },
	{ "top_right", GTK_CORNER_TOP_RIGHT },
	{ "bottom_right", GTK_CORNER_BOTTOM_RIGHT },
};	/* GtkCornerType */
#endif

/* Packing types (for boxes) */
static EnumTypeDef pack[] = {
	{ "start", GTK_PACK_START },
	{ "end", GTK_PACK_END },
};	/* GtkPackType */

#if	GTK_1_2
/* priorities for path lookups */
static EnumTypeDef path_prio[] = {
	{ "low", GTK_PATH_PRIO_LOWEST },
	{ "gtk", GTK_PATH_PRIO_GTK },
	{ "app", GTK_PATH_PRIO_APPLICATION },
	{ "rc", GTK_PATH_PRIO_RC },
	{ "high", GTK_PATH_PRIO_HIGHEST },
	{ "mask", GTK_PATH_PRIO_MASK },
};	/* GtkPathPriorityType */

/* widget path types */
static EnumTypeDef path[] = {
	{ "widget", GTK_PATH_WIDGET },
	{ "widget_class", GTK_PATH_WIDGET_CLASS },
	{ "class", GTK_PATH_CLASS },
};	/* GtkPathType */
#endif

/* Scrollbar policy types (for scrolled windows) */
static EnumTypeDef policy[] = {
	{ "always", GTK_POLICY_ALWAYS },
	{ "auto", GTK_POLICY_AUTOMATIC },
#if	GTK_1_2
	{ "never", GTK_POLICY_NEVER },
#endif
};	/* GtkPolicyType */

static EnumTypeDef position[] = {
	{ "left", GTK_POS_LEFT },
	{ "right", GTK_POS_RIGHT },
	{ "top", GTK_POS_TOP },
	{ "bottom", GTK_POS_BOTTOM },
};	/* GtkPositionType */

static EnumTypeDef preview[] = {
	{ "color", GTK_PREVIEW_COLOR },
	{ "gray", GTK_PREVIEW_GRAYSCALE },
};	/* GtkPreviewType */

#if	GTK_1_2
/* Style for buttons */
static EnumTypeDef relief[] = {
	{ "normal", GTK_RELIEF_NORMAL },
	{ "half", GTK_RELIEF_HALF },
	{ "none", GTK_RELIEF_NONE },
};	/* GtkReliefStyle */

/* Resize type */
static EnumTypeDef resize[] = {
	{ "parent", GTK_RESIZE_PARENT },
	{ "queue", GTK_RESIZE_QUEUE },
	{ "immediate", GTK_RESIZE_IMMEDIATE },
};	/* GtkResizeMode */
#endif

/* signal run types */
static EnumTypeDef run[] = {
	{ "first", GTK_RUN_FIRST },
	{ "last", GTK_RUN_LAST },
	{ "both", GTK_RUN_BOTH },
	{ "no_recurse", GTK_RUN_NO_RECURSE },
#if	GTK_1_2
	{ "action", GTK_RUN_ACTION },
	{ "no_hooks", GTK_RUN_NO_HOOKS },
#endif
};	/* GtkSignalRunType */

/* scrolling types */
static EnumTypeDef scroll[] = {
	{ "none", GTK_SCROLL_NONE },
	{ "step_backward", GTK_SCROLL_STEP_BACKWARD },
	{ "step_forward", GTK_SCROLL_STEP_FORWARD },
	{ "page_backward", GTK_SCROLL_PAGE_BACKWARD },
	{ "page_foreward", GTK_SCROLL_PAGE_FORWARD },
	{ "scroll_jump", GTK_SCROLL_JUMP },
};	/* GtkScrollType */

/* list selection modes */
static EnumTypeDef selection[] = {
	{ "single", GTK_SELECTION_SINGLE },
	{ "browse", GTK_SELECTION_BROWSE },
	{ "multiple", GTK_SELECTION_MULTIPLE },
	{ "extended", GTK_SELECTION_EXTENDED },
};	/* GtkSelectionMode */

/* Shadow types */
static EnumTypeDef shadow[] = {
	{ "none", GTK_SHADOW_NONE },
	{ "in", GTK_SHADOW_IN },
	{ "out", GTK_SHADOW_OUT },
	{ "etched_in", GTK_SHADOW_ETCHED_IN },
	{ "etched_out", GTK_SHADOW_ETCHED_OUT },
};	/* GtkShadowType */

/* Widget states */
static EnumTypeDef state[] = {
	{ "normal", GTK_STATE_NORMAL },
	{ "active", GTK_STATE_ACTIVE },
	{ "prelight", GTK_STATE_PRELIGHT },
	{ "selected", GTK_STATE_SELECTED },
	{ "intensitive", GTK_STATE_INSENSITIVE },
};	/* GtkStateType */

/* Directions for submenus */
static EnumTypeDef sub_dir[] = {
	{ "left", GTK_DIRECTION_LEFT },
	{ "right", GTK_DIRECTION_RIGHT },
};	/* GtkSubmenuDirection */

/* Placement of submenus */
static EnumTypeDef sub_place[] = {
	{ "top_bottom", GTK_TOP_BOTTOM },
	{ "left_right", GTK_LEFT_RIGHT },
};	/* GtkSubmenuPlacement */

/* Style for toolbars */
static EnumTypeDef toolbar[] = {
	{ "icons", GTK_TOOLBAR_ICONS },
	{ "text", GTK_TOOLBAR_TEXT },
	{ "both", GTK_TOOLBAR_BOTH },
};	/* GtkToolbarStyle */

/* Trough types for GtkRange */
static EnumTypeDef through[] = {
	{ "none", GTK_TROUGH_NONE },
	{ "start", GTK_TROUGH_START },
	{ "end", GTK_TROUGH_END },
	{ "jump", GTK_TROUGH_JUMP },
};	/* GtkTroughType */

/* Data update types (for ranges) */
static EnumTypeDef update[] = {
	{ "continuous", GTK_UPDATE_CONTINUOUS },
	{ "discontinuous", GTK_UPDATE_DISCONTINUOUS },
	{ "delayed", GTK_UPDATE_DELAYED },
};	/* GtkUpdateType */

/* Generic visibility flags */
static EnumTypeDef visibility[] = {
	{ "none", GTK_VISIBILITY_NONE },
	{ "partial", GTK_VISIBILITY_PARTIAL },
	{ "full", GTK_VISIBILITY_FULL },
};	/* GtkVisibility */

/* Window position types */
static EnumTypeDef win_pos[] = {
	{ "none", GTK_WIN_POS_NONE },
	{ "center", GTK_WIN_POS_CENTER },
	{ "mouse", GTK_WIN_POS_MOUSE },
#if	GTK_1_2
	{ "center_always", GTK_WIN_POS_CENTER_ALWAYS },
#endif
};	/* GtkWindowPosition */

/* Window types */
static EnumTypeDef window[] = {
	{ "toplevel", GTK_WINDOW_TOPLEVEL },
	{ "dialog", GTK_WINDOW_DIALOG },
	{ "popup", GTK_WINDOW_POPUP },
};	/* GtkWindowType */

#if	GTK_1_2
/* How to sort */
static EnumTypeDef sort[] = {
	{ "ascending", GTK_SORT_ASCENDING },
	{ "descending", GTK_SORT_DESCENDING },
};	/* GtkSortType */
#endif

/*
:de:
Die Funktion |$1| erweitert den esh-Interpreter zur Nutzung
der Gtk+ Aufzählungstypen.
:*:
The function |$1| expands the esh-Interpreter for use of the
Gtk+ enum types.
*/

void EGtkEnum_setup (void)
{
	NewEnumType("GtkArrowType", arrow, tabsize(arrow));
	NewEnumType("GtkAttachOptions", attach, tabsize(attach));
	NewEnumType("GtkDirectionType", direction, tabsize(direction));
	NewEnumType("GtkJustification", justification, tabsize(justification));
	NewEnumType("GtkMenuFactoryType", menu_factory, tabsize(menu_factory));
	NewEnumType("GtkMetricType", metric, tabsize(metric));
	NewEnumType("GtkOrientation", orientation, tabsize(orientation));
	NewEnumType("GtkPackType", pack, tabsize(pack));
	NewEnumType("GtkPolicyType", policy, tabsize(policy));
	NewEnumType("GtkPositionType", position, tabsize(position));
	NewEnumType("GtkPreviewType", preview, tabsize(preview));
	NewEnumType("GtkSignalRunType", run, tabsize(run));
	NewEnumType("GtkScrollType", scroll, tabsize(scroll));
	NewEnumType("GtkSelectionMode", selection, tabsize(selection));
	NewEnumType("GtkShadowType", shadow, tabsize(shadow));
	NewEnumType("GtkStateType", state, tabsize(state));
	NewEnumType("GtkSubmenuDirection", sub_dir, tabsize(sub_dir));
	NewEnumType("GtkSubmenuPlacement", sub_place, tabsize(sub_place));
	NewEnumType("GtkToolbarStyle", toolbar, tabsize(toolbar));
	NewEnumType("GtkTroughType", through, tabsize(through));
	NewEnumType("GtkUpdateType", update, tabsize(update));
	NewEnumType("GtkVisibility", visibility, tabsize(visibility));
	NewEnumType("GtkWindowPosition", win_pos, tabsize(win_pos));
	NewEnumType("GtkWindowType", window, tabsize(window));
#if	GTK_1_2
	NewEnumType("GtkButtonBoxStyle", buttonbox, tabsize(buttonbox));
	NewEnumType("GtkCurveType", curve, tabsize(curve));
	NewEnumType("GtkCornerType", corner, tabsize(corner));
	NewEnumType("GtkMatchType", match, tabsize(match));
	NewEnumType("GtkMenuDirectionType", menu_dir, tabsize(menu_dir));
	NewEnumType("GtkPathPriorityType", path_prio, tabsize(path_prio));
	NewEnumType("GtkPathType", path, tabsize(path));
	NewEnumType("GtkReliefStyle", relief, tabsize(relief));
	NewEnumType("GtkResizeMode", resize, tabsize(resize));
	NewEnumType("GtkSortType", sort, tabsize(sort));
#endif
}

/*
$SeeAlso
\mref{EGtk(7)}.
*/
