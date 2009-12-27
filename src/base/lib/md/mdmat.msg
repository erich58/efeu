/*	Formatdefinitionen für MDMAT-Programmbibliothek
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

1	#Name	Zahl	Beschreibung\n

10	$!: $0: ACHTUNG: Altes Datenformat.\n
11	$!: Falsche Magic-Nummer in Datenfile.\n
12	$!: Falsche Elementzahl in Datenfile.\n
13	$!: EOF - Marke fehlt.\n
14	$!: Falsche Datenlänge für Objekttype $1.\n
15	$!: Datentype $1 hate keine Komponente $2.\n
16	$!: Datentype $1 kann nicht addiert werden (Operator += nicht definiert).\n
17	$!: Achtung: Datentype $1 mit variabler Satzlänge.\n

18	$!: Binärer Operator $1 ($2, $3) ist nicht definiert.\n
19	$!: Zuweisungsoperator $2::$1 & ($3) ist nicht definiert.\n

21	$!: Objekttyp $1 nicht definiert.\n
22	$!: Datentype $1 kann nicht gelesen werden.\n
23	$!: Eine Komponente des Datentypes $(#1:%#s) kann nicht gelesen werden.\n
24	$!: Datentype $1 kann nicht ausgegeben werden.\n
25	$!: Eine Komponente des Datentypes $(#1:%#s) kann nicht ausgegeben werden.\n

31	$!: Falsche Kennung in Datenfile.\n
32	$!: Fehlerhafte Dimensionsangabe.\n
33	$!: Datensektor fehlt.\n

41	$!: Zählerstruktur nicht definiert.\n
42	$!: Additionsfunktion nicht definiert.\n
43	$!: Gruppendefinitionen fehlen.\n
44	$!: Gruppendefinition $1 nicht gefunden.\n

51	$!: $(#1:%#s) ist keine Gleitkommamatrix.\n
52	$!: Gleitkommamatrix $(#1:%#s) - falsche Objektgröße.\n
53	$!: Multiplikation von $(#1:%#s) mit $(#2:%#s):\n\
Dimensionenszahlen sind nicht kompatibel.\n
54	$!: Multiplikation von $(#1:%#s) mit $(#2:%#s):\n\
Achsenindizes sind nicht kompatibel.\n

61	$!: Leere Matrize(n) bei Matrixmultiplikation.\n
62	$!: Matrixmultiplikation: Achsen $(#1:%#s) - $(#2:%#s) sind nicht kompatibel (flags=$3).\n
63	$!: Matrixmultiplikation: Inkompatible Achsenzahl.\n
64	$!: Matrixmultiplikation: Datentype $1 nicht mit $2 multiplizierbar.\n
65	$!: Matrixmultiplikation: Datentype $1 nicht addierbar.\n

71	$!: $0: Spaltenbezeichner fehlerhaft.\n
72	$!: $0: Unvollständige Zeile.\n
73	$!: $0: Extra Zeichen: $(#1:%#s).\n

81	$!: Zeichen $1 statt '=' in Listendefinition.\n

91	$!: mdterm: Funktion ist nicht definiert.\n
92	$!: Achsen $1 - $1 sind nicht kompatibel (andere Dimension).\n
93	$!: mdterm: Funktion %s hat keinen Rückgabewert.\n
94	$!: mdterm: Funktion %s ist nicht definiert.\n
95	$!: Funktion $1 ist nicht zur Datenabfrage vom Type $2 geeignet.\n

101	$!: Datenmatrix hat keine Komponente $(#1:%#s).\n
102	$!: Type $(#1:%#s) nicht in $(#2:%#s) konvertierbar.\n
103	$!: Datenmatrix von Type $(#1:%#s) kann nicht gerundet werden.\n

191	$!: $0: Unvollständige Zeile.\n
192	$!: $0: Unerwartetes Fileende, keine Daten.\n
193	$!: $0: Spaltenbezeichner fehlt.\n
194	$!: $0: Verschiedene Achsenbezeicher ($(#1:%#s) - $(#2:%#s))\nbei gleichem Datenwert.\n
195	$!: $0: Achsenbezeichner unvollständig.\n
196	$!: $0: Extra Werte in Datenfile.\n
197	$!: $0: Illegale Zeichenfolge $(#1:%#s) im Zahlenwert.\n

201	$!: Leere Matrix bei Matrixinversion.\n
202	$!: Type $(#1:%#s) nicht in double konvertierbar.\n
203	$!: Matrixinversion: Matrix nicht Quadratisch.\n
204	$!: Matrixinversion: Datenmatrix ist singulär (dim=$1, rang=$2).\n
