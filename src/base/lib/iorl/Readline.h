/*
Readline-Schnittstelle

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_Readline_h
#define	EFEU_Readline_h	1

#include <EFEU/io.h>

extern void SetupReadline (void);

/*
$Description
Die Readline-Schnittstelle läßt sich mithilfe von |io_ctrl|-Aufrufen
konfigurieren. Alls erstes Argument wird eine IO-Struktur übergeben,
danach folgt der Kontrollmakro und die zur Steuerung notwendigen
Argumente. Bei Erfolg liefert der Aufruf den Wert von 0.
Falls die <io>-Struktur keine Readline-Schnittstelle darstellt,
werden die Aktionen einfach ignoriert und |io_ctrl| liefert |EOF| (-1).

Im folgenden sind die einzelnen Kontrollaufrufe angeführt.
Der Datentype der spezifischen Argumente wird mit einem cast dargestellt:

[<|io_ctrl(<io>, $1)|>]
	bewirkt nichts, der Rückgabewert kann aber
	zum testen einer IO-Struktur auf eine
	Readline-Schnittstelle verwendet werden.
	Eine Readline-Schnittstelle wird nur für interaktive Programme
	eingerichtet!
*/

#define	RLCTRL			('R' << 8)

/*
[<|io_ctrl(<io>, $1, (int) <key>|>)]
	setzt das Interaktionszeichen für eingebaute Befehle der
	Readline-Schnittstelle.
	Voreingestellt ist das Rufzeichen <"|!|">.
*/

#define	RLCTRL_KEY		(RLCTRL|0x01)

/*
[<|io_ctrl(<io>, $1, (int) <size>|>)]
	legt die maximale Zahl der History-Zeilen festgelgt.
	Ein Wert von 0 steht für unbegrenzt.
	Voreingestellt sind 100 Zeilen.
*/

#define	RLCTRL_HSIZE		(RLCTRL|0x02)

/*
$Example
Die nachfolgenden Programmzeilen zeigen die typische Verwendung der
oben beschriebenen Steuermakros.

---- verbatim
if	(io_ctrl(io, RLCTRL) == 0)
	io_puts("readline-Funktionalität verfügbar!\n", io);

io_ctrl(io, RLCTRL_KEY, '%');
io_ctrl(io, RLCTRL_HSIZE, 500);
----
*/

#endif	/* EFEU_Readline_h */
