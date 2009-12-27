include efeu.smh
include gtk.smh

mf_var CFLAGS '$(GTK_INC)'
efeu_library -u -h GUI Gtk -ml EFX_LIB -ml GTK_LIB
