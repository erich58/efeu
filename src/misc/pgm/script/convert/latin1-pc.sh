#	Filter zur Konvertierung von ISO-Latin 1 in PC - Zeichensatz
#	

IDEF="������߫�"
ODEF="������ᮯ"

unset LANG
tr "$IDEF" "$ODEF"
