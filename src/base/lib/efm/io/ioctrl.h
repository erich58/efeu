/*	Standard - Kontrollfunktionen für IO-Strukturen
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

$Header	<EFEU/$1>
*/

#ifndef	EFEU_IOCTRL_H
#define	EFEU_IOCTRL_H	1

#include <EFEU/io.h>

#define	IO_STDCTRL		('S' << 8)

/*	Basiskontrollfunktionen
*/

#define	IO_CLOSE		(IO_STDCTRL|0x01)
#define	IO_UNGETC		(IO_STDCTRL|0x02)
#define	IO_FLUSH		(IO_STDCTRL|0x03)
#define	IO_REWIND		(IO_STDCTRL|0x04)
#define	IO_ERROR		(IO_STDCTRL|0x05)
#define	IO_PROTECT		(IO_STDCTRL|0x06)
#define	IO_RESTORE		(IO_STDCTRL|0x07)
#define	IO_PEEK			(IO_STDCTRL|0x08)
#define	IO_STAT			(IO_STDCTRL|0x09)

#define	IO_IDENT		(IO_STDCTRL|0x11)
#define	IO_LINE			(IO_STDCTRL|0x12)
#define	IO_COL			(IO_STDCTRL|0x13)
#define	IO_SUBMODE		(IO_STDCTRL|0x14)
#define	IO_PROMPT		(IO_STDCTRL|0x15)
#define	IO_LINEMARK		(IO_STDCTRL|0x16)

/*	Terminalkontrolle
*/

#define	IO_LINES		(IO_STDCTRL|0x21)
#define	IO_COLUMNS		(IO_STDCTRL|0x22)

#define	IO_ROMAN_FONT		(IO_STDCTRL|0x23)
#define	IO_BOLD_FONT		(IO_STDCTRL|0x24)
#define	IO_ITALIC_FONT		(IO_STDCTRL|0x25)

/*	Stackkontrolle
*/

#define	IOSTACK_PROTECT		(IO_STDCTRL|0x30)
#define	IOSTACK_PUSH		(IO_STDCTRL|0x31)
#define	IOSTACK_POP		(IO_STDCTRL|0x32)
#define	IOSTACK_GET		(IO_STDCTRL|0x33)

/*	Preprozesskontrolle
*/

#define	IOPP_COMMENT		(IO_STDCTRL|0x41)
#define	IOPP_EXPAND		(IO_STDCTRL|0x42)

/*	Dokumentformatierung
*/

#define	DOCOUT_REM		(IO_STDCTRL|0x50)	/* Anmerkung */
#define	DOCOUT_SYM		(IO_STDCTRL|0x51)	/* Sonderzeichen */
#define	DOCOUT_CMD		(IO_STDCTRL|0x52)	/* Dokumentbefehl */

#endif	/* EFEU/ioctrl.h */
