#	Filter zur Konvertierung von PC-Zeichensatz in ISO-Latin 1 Zeichensatz
#	

IDEF="������ᮯ"
ODEF="������߫�"

unset LANG
tr "$IDEF" "$ODEF"
