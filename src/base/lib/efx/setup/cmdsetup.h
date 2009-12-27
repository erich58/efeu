/*
Befehlsinterpreter initialisieren

$Header <EFEU/$1>

$Copyright (C) 1996 Erich Frühstück
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

#ifndef	EFEU_cmdsetup_h
#define	EFEU_cmdsetup_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/stdtype.h>

void CmdSetup_const(void);	/* Konstanten */
void CmdSetup_base(void);	/* Basistypen */
void CmdSetup_int(void);	/* Ganzzahlwerte */
void CmdSetup_uint(void);	/* Vorzeichenfreie Ganzzahlwerte */
void CmdSetup_size(void);	/* Vorzeichenfreie lange Ganzzahlwerte */
void CmdSetup_float(void);	/* Einfache Gleitkommawerte */
void CmdSetup_double(void);	/* Doppelte Gleitkommawerte */
void CmdSetup_str(void);	/* Zeichen und Strings */
void CmdSetup_varstr(void);	/* Zeichenketten, Version 2 */
void CmdSetup_io(void);		/* Ein-, Ausgabestrukturen */
void CmdSetup_print(void);	/* Ausgabefunktionen */
void CmdSetup_vec(void);	/* Vektorfunktionen */
void CmdSetup_obj(void);	/* Universielle Operatoren */
void CmdSetup_range(void);	/* Bereichsfunktionen */
void CmdSetup_pctrl(void);	/* Programmsteuerung */
void CmdSetup_signal(void);	/* Signalhaendling */
void CmdSetup_test(void);	/* Testfunktionen */
void CmdSetup_konv(void);	/* Konvertierungen */
void CmdSetup_func(void);	/* Funktionen */
void CmdSetup_op(void);		/* Operatoren einrichten */
void CmdSetup_info(void);	/* Informationsdatenbank */
void CmdSetup_strbuf(void);	/* Stringbuffer */
void CmdSetup_xml(void);	/* XML-Creator */
void CmdSetup_StrPool(void);	/* Stringpool */
void CmdSetup_DatPool(void);	/* Datenpool */
void CmdSetup_MapFile(void);	/* Dateimapping */
void CmdSetup_assignarg(void);	/* Zuweisungsargument */
void CmdSetup_date(void);	/* Datum- und Zeit */
void CmdSetup_unix(void);	/* UNIX-Systemstrukturen */
void CmdSetup_regex(void);	/* Reguläre Ausdrücke */
void CmdSetup_match(void);	/* Mustervergleich */
void CmdSetup_cmdpar(void);	/* Befehlsparameter */
void CmdSetup_dl(void);		/* Dynamische Bibliotheken */
void CmdSetup_dbutil(void);	/* Datenbankhilfsfunktionen */
void CmdSetup_stat(void);	/* Interpreterstatus */
void CmdSetup_type(void);	/* Datentypen */

void CmdSetup_int8 (void);
void CmdSetup_int16 (void);
void CmdSetup_int32 (void);
void CmdSetup_int64 (void);
void CmdSetup_varint (void);

void CmdSetup_uint8 (void);
void CmdSetup_uint16 (void);
void CmdSetup_uint32 (void);
void CmdSetup_uint64 (void);
void CmdSetup_varsize (void);

#endif	/* EFEU/cmdsetup.h */
