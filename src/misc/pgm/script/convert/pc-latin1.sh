#	Filter zur Konvertierung von PC-Zeichensatz in ISO-Latin 1 Zeichensatz
#	

IDEF="™š„”á®¯"
ODEF="ÄÖÜäöüß«»"

unset LANG
tr "$IDEF" "$ODEF"
