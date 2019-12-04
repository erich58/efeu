# :*:make configuration for tk interface
# :de:make Konfiguration für tk Schnittstelle
#
# $Synopsis
# include $1
# \par
# \$(TK_INC)\br
# \$(TK_LIB)
#
# $Copyright (C) 2019 Erich Frühstück
# This file is part of EFEU.
# 
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
 
ifndef _tklib_mak_
_tklib_mak_ = defined

# $Description
# :*:
# Including the header |$1| defines the |make|-makros \index{|TK_INC|}
# and \index{|TK_LIB|} with compile and link flags needed for Tcl/Tk
# applications.
# The configuration is done with the command lines:
# :de:
# Durch Einbinden der Headerdatei |$1| werden die |make|-Makros
# \index{|TK_INC|} und \index{|TK_LIB|} mit Kompiler und Linkeroptionen
# für Tcl/Tk Anwendungen definiert.
# Die Konfiguration erfolgt mit den Befehlszeilen:
#
# $source

@beg cflags
TK_INC = -I/usr/include//:/usr/local/include tk.h

TK_LIB = -L/usr/lib:/usr/local/lib -ltk[0-9.]*
TK_LIB += -L/usr/X11R6/lib:/usr/local/lib -lX11
TK_LIB += -lm -ldl
@end

endif
