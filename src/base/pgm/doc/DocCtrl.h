/*
Kontrollfunktionen für Dokumentausgabe

$Copyright (C) 1991 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	_EFEU_DocCtrl_h
#define	_EFEU_DocCtrl_h	1

#ifndef	CFGEXT
#define	CFGEXT	"cfg"
#endif

#define	DOC_CTRL	('D' << 8)
#define	DOC_TYPE	(DOC_CTRL|0)	/* Dokumenttype */
#define	DOC_HEAD	(DOC_CTRL|1)	/* Dokumentkopf */
#define	DOC_REM		(DOC_CTRL|2)	/* Anmerkungen */
#define	DOC_SYM		(DOC_CTRL|3)	/* Sonderzeichen */

/*	Steuerbefehle
*/

#define	DOC_CMD		(DOC_CTRL|10)	/* Dokumentbefehl */

#define	DOC_CMD_BREAK	0x01	/* Zeilenumbruch */
#define	DOC_CMD_NPAGE	0x02	/* Neue Seite */
#define	DOC_CMD_TITLE	0x03	/* Titelblatt */
#define	DOC_CMD_TOC	0x04	/* Inhaltsverzeichnis */
#define	DOC_CMD_LOF	0x05	/* Verzeichnis der Bilder */
#define	DOC_CMD_LOT	0x06	/* Verzeichnis der Tabellen */
#define	DOC_CMD_ITEM	0x07	/* Neuer Listeneintrag */
#define	DOC_CMD_APP	0x08	/* Beginn des Anhangs */
#define	DOC_CMD_IDX	0x09	/* Indexeintrag */
#define	DOC_CMD_MARK	0x0a	/* Fußnotenmarke */
#define	DOC_CMD_TEX	0x0b	/* TeX-Kommando */

/*	Referenzen
*/

#define	DOC_REF_LABEL	0x10	/* Label */
#define	DOC_REF_STD	0x11	/* Standardreferenz */
#define	DOC_REF_PAGE	0x12	/* Referenz auf Seite */
#define	DOC_REF_VAR	0x13	/* Kombinierte Referenz */
#define	DOC_REF_MAN	0x14	/* Referenz auf Handbuch */

/*	Tabellen
*/

#define	DOC_TAB_BEG	0x20	/* Beginn einer Tabellenzeile */
#define	DOC_TAB_SEP	0x21	/* Tabellentrennzeichen */
#define	DOC_TAB_END	0x22	/* Ende einer Tabellenzeile */
#define	DOC_TAB_HEIGHT	0x23	/* Tabellenhöhe */
#define	DOC_TAB_BLINE	0x24	/* Fette Zeile */
#define	DOC_TAB_BRULE	0x25	/* Fette Linie */
#define	DOC_TAB_INDENT	0x26	/* Tabelleneinrückung */

/*	Umgebungen werden mit DOC_BEG eingeleitet und mit DOC_END
	beendet.  Die Umgebungskennungen werden nach inhaltlichen
	Gruppen zusammengefaßt.
*/

#define	DOC_BEG		(DOC_CTRL|11)	/* Beginn einer Umgebung */
#define	DOC_END		(DOC_CTRL|12)	/* Ende einer Umgebung */


/*	Absatzformen
*/

#define	DOC_PAR_CTRL	0x00			/* Kennung */
#define	DOC_PAR_STD	(DOC_PAR_CTRL|0x0)	/* Gewöhnlicher Absatz */
#define	DOC_PAR_ITEM	(DOC_PAR_CTRL|0x1)	/* Listeneintrag */
#define	DOC_PAR_TAG	(DOC_PAR_CTRL|0x2)	/* Beschreibungslabel */

/*	Gliederungsbefehle
*/

#define	DOC_SEC_CTRL	0x10			/* Kennung */
#define	DOC_SEC_PART	(DOC_SEC_CTRL|0x0)	/* Dokumentteil */
#define	DOC_SEC_CHAP	(DOC_SEC_CTRL|0x1)	/* Kapitel */
#define	DOC_SEC_MCHAP	(DOC_SEC_CTRL|0x2)	/* Handbuchkapitel */
#define	DOC_SEC_SECT	(DOC_SEC_CTRL|0x3)	/* Bereich */
#define	DOC_SEC_SSECT	(DOC_SEC_CTRL|0x4)	/* Zwischentitel */
#define	DOC_SEC_PARA	(DOC_SEC_CTRL|0x5)	/* Paragraph */
#define	DOC_SEC_HEAD	(DOC_SEC_CTRL|0x6)	/* Hauptüberschrift */
#define	DOC_SEC_SHEAD	(DOC_SEC_CTRL|0x7)	/* Nebenüberschrift */
#define	DOC_SEC_CAPT	(DOC_SEC_CTRL|0x8)	/* Tabellenüberschrift */
#define	DOC_SEC_SCAPT	(DOC_SEC_CTRL|0x9)	/* Tabellennenenüberschrift */
#define	DOC_SEC_MARG	(DOC_SEC_CTRL|0xa)	/* Randnotiz */
#define	DOC_SEC_NOTE	(DOC_SEC_CTRL|0xb)	/* Anmerkung */
#define	DOC_SEC_FNOTE	(DOC_SEC_CTRL|0xc)	/* Tabellenanmerkung */

/*	Listen
*/

#define	DOC_LIST_CTRL	0x20			/* Kennung */
#define	DOC_LIST_ITEM	(DOC_LIST_CTRL|0x0)	/* Aufzählungsliste */
#define	DOC_LIST_ENUM	(DOC_LIST_CTRL|0x1)	/* Numerierungsliste */
#define	DOC_LIST_DESC	(DOC_LIST_CTRL|0x2)	/* Beschreibungsliste */

#define	DOC_IS_LIST(x)	(((x) & ~0xf) == DOC_LIST_CTRL)

/*	Attribute
*/

#define	DOC_ATT_CTRL	0x30			/* Kennung */
#define	DOC_ATT_RM	(DOC_ATT_CTRL|0x0)	/* Roman */
#define	DOC_ATT_IT	(DOC_ATT_CTRL|0x1)	/* Italic */
#define	DOC_ATT_BF	(DOC_ATT_CTRL|0x2)	/* Fett */
#define	DOC_ATT_TT	(DOC_ATT_CTRL|0x3)	/* Schreibmaschine */

/*	Anführungen
*/

#define	DOC_QUOTE_CTRL	0x40			/* Kennung */
#define	DOC_QUOTE_SGL	(DOC_QUOTE_CTRL|0x0)	/* Einfache Anführung */
#define	DOC_QUOTE_DBL	(DOC_QUOTE_CTRL|0x1)	/* Doppelte Anführung */

/*	Verarbeitungsmodi
*/

#define	DOC_MODE_CTRL	0x50			/* Kennung */
#define	DOC_MODE_SKIP	(DOC_MODE_CTRL|0x0)	/* Ignorieren */
#define	DOC_MODE_COPY	(DOC_MODE_CTRL|0x1)	/* Kopiermodus */
#define	DOC_MODE_PLAIN	(DOC_MODE_CTRL|0x2)	/* Rohausgabe */
#define	DOC_MODE_VERB	(DOC_MODE_CTRL|0x3)	/* Verbatim */
#define	DOC_MODE_MAN	(DOC_MODE_CTRL|0x4)	/* Handbuchsource */
#define	DOC_MODE_TEX	(DOC_MODE_CTRL|0x5)	/* TeX-Source */
#define	DOC_MODE_HTML	(DOC_MODE_CTRL|0x6)	/* HTML-Source */

/*	Sonstige Umgebungen
*/

#define	DOC_ENV_CTRL	0x60			/* Kennung */
#define	DOC_ENV_BASE	(DOC_ENV_CTRL|0x0)	/* Standardumgebung */
#define	DOC_ENV_INTRO	(DOC_ENV_CTRL|0x1)	/* Einleitung */
#define	DOC_ENV_QUOTE	(DOC_ENV_CTRL|0x2)	/* Einrückung */
#define	DOC_ENV_FORMULA	(DOC_ENV_CTRL|0x4)	/* Formelsatz */
#define	DOC_ENV_TABLE	(DOC_ENV_CTRL|0x5)	/* Tabellenumgebung */
#define	DOC_ENV_FIG	(DOC_ENV_CTRL|0x6)	/* Bildumgebung */
#define	DOC_ENV_TAB	(DOC_ENV_CTRL|0x7)	/* Tabelle */
#define	DOC_ENV_URL	(DOC_ENV_CTRL|0x8)	/* URL */
#define	DOC_ENV_CODE	(DOC_ENV_CTRL|0x9)	/* Befehlszeilen */
#define	DOC_ENV_HANG	(DOC_ENV_CTRL|0xa)	/* Hängende Absätze */
#define	DOC_ENV_SLOPPY	(DOC_ENV_CTRL|0xb)	/* Schlampige Formatierung */
#define	DOC_ENV_MCOL	(DOC_ENV_CTRL|0xc)	/* Mehrfachspalte */
#define	DOC_ENV_SPAGE	(DOC_ENV_CTRL|0xd)	/* Teilseite */
#define	DOC_ENV_MPAGE	(DOC_ENV_CTRL|0xe)	/* Handbucheintrag */
#define	DOC_ENV_BIB	(DOC_ENV_CTRL|0xf)	/* Literaturverzeichnis */

#endif	/* EFEU/DocCtrl.h */
