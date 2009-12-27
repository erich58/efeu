#	Filter zur Konvertierung von ISO-Latin 1 in PC - Zeichensatz
#	

IDEF="ÄÖÜäöüß«»"
ODEF="™š„”á®¯"

unset LANG
tr "$IDEF" "$ODEF"
