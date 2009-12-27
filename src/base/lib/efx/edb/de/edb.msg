/*
Fehlermeldungen für edb-Funktionen

$Copyright (C) 2005 Erich Frühstück
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

cmd	$!: Unbekanntes Kommando \"$1\"\n
cmp	$!: Sortierfehler bei Datenbank $1.\n
conv	$!: Datetyp $1 nicht in $2 konvertierbar.\n
aggr	$!: Aggregatfunktion $1 für Datentyp $2 ist nicht defininiert.\n
dbi	$!: Ungültige Datenfileangabe $1.\n
inc	$!: Schlüssel $1 nicht kompatibel.\n
key	$!: Schlüssel ist nicht definiert.\n
mem	$!: Datentype $1 hat keine Komponente $2.\n
mhdr	$0: Falsche Dateikennung.\n
noc	$!: Verwendung eines C-Ausdruck ist nicht verfügbar.\n
ord	$!: Unzulässige Reihenfolge der Verknüpfungsparameter.\n
type	$0: Datentyp ist nicht definiert.\n
typ	$!: Unbekannter Datentyp $1.\n
uchar	$0: Uberwartetes Zeichen $1.\n
use	$!: Name $1 ist bereits verwendet.\n
var	$!: Ungültige Variablendefinition $1.\n
vec	$!: Vektor kann nur zur Gänze selektiert werden.\n
xchar 	$0: Extra Zeichen im Header ignoriert.\n
print	$!: Datentyp $1: Unbekannte Ausgabedefinition $2%#s.\n
input	$!: Datentyp $1: Unbekannte Eingabedefinition $2%#s.\n

nread	$0: $1 Datensätze gelesen.\n
nwrite	$0: $1 Datensätze geschrieben.\n
