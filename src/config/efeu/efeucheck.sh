#!/bin/sh
# :*:check software configuration
# :de:Prüfen von Softwareverfügbarkeit
#
# $Copyright (C) 2016 Erich Frühstück
# This file is part of EFEU.
# 
# EFEU is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# EFEU is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with EFEU; see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

# create temporary working directory

tmp=`mktemp -d`
trap "rm -rf $tmp;" 0
trap "exit 1" 1 2 3 15
readonly tmp

list_cmd()
{
	awk '
/[)][ \t]*#/ {
	match($0, "[)][ \t]*#[ \t]*");
	name = substr($0, 1, RSTART - 1);
	desc = substr($0, RSTART + RLENGTH);

	tags = tags ? tags " | " name : name
	doc = doc name "\n\t" desc "\n";
	tab[name] = desc

	if	(len < length(name))
		len = length(name);
}
END {
for (x in tab)
	printf("%-*s %s\n", len, x, tab[x]);
}
' $1 | sort
}

case "$1" in
latex)	# check availability of latex
	cd $tmp
	cat > ltest.tex << !
\\documentclass[10pt,a4paper]{article}
\\usepackage{ucs}
\\usepackage[utf8x]{inputenc}
\\usepackage{tabularx}
\\usepackage{german}
\\usepackage[german]{varioref}
\\dateaustrian
\\begin{document}
This is a Test.
\\end{document}
!
	latex ltest.tex >/dev/null 2>&1 </dev/null && \
		dvips -o ltest.ps ltest.dvi 2>/dev/null
	;;
ps2pdf)	# check availability of ps2pdf
	printf '%%!PS\nshowpage\n' | ps2pdf - - >/dev/null
	;;
pdfdoc)	# efeudoc supports pdf output
	echo "Test" | efeudoc --pdf - >/dev/null 2>&1
	;;
latexdoc) # efeudoc supports postscript with latex
	echo "Test" | efeudoc -p - >/dev/null 2>&1
	;;
?*)
	exit 1
	;;
*)
	printf 'Usage: %s what\n\n' $0
	list_cmd $0
	;;
esac
