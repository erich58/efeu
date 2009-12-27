/*
Standard - Kontrollfunktionen für IO-Strukturen

$Header	<EFEU/$1>

$Copyright (C) 1997 Erich Frühstück
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

#ifndef	EFEU_ioctrl_h
#define	EFEU_ioctrl_h	1

#include <EFEU/io.h>

#define	IO_STDCTRL		('S' << 8)

#define	IO_CLOSE		(IO_STDCTRL|0x01)
#define	IO_UNGETC		(IO_STDCTRL|0x02)
#define	IO_FLUSH		(IO_STDCTRL|0x03)
#define	IO_REWIND		(IO_STDCTRL|0x04)
#define	IO_ERROR		(IO_STDCTRL|0x05)
#define	IO_PROTECT		(IO_STDCTRL|0x06)
#define	IO_RESTORE		(IO_STDCTRL|0x07)
#define	IO_PEEK			(IO_STDCTRL|0x08)
#define	IO_STAT			(IO_STDCTRL|0x09)
#define	IO_GETPOS		(IO_STDCTRL|0x0a)
#define	IO_SETPOS		(IO_STDCTRL|0x0b)
#define	IO_FDIN			(IO_STDCTRL|0x0c)
#define	IO_FDOUT		(IO_STDCTRL|0x0d)

#define	IO_IDENT		(IO_STDCTRL|0x11)
#define	IO_LINE			(IO_STDCTRL|0x12)
#define	IO_COL			(IO_STDCTRL|0x13)
#define	IO_SUBMODE		(IO_STDCTRL|0x14)
#define	IO_PROMPT		(IO_STDCTRL|0x15)
#define	IO_LINEMARK		(IO_STDCTRL|0x16)

#define	IO_LINES		(IO_STDCTRL|0x21)
#define	IO_COLUMNS		(IO_STDCTRL|0x22)

#define	IO_ROMAN_FONT		(IO_STDCTRL|0x23)
#define	IO_BOLD_FONT		(IO_STDCTRL|0x24)
#define	IO_ITALIC_FONT		(IO_STDCTRL|0x25)

#define	IO_NEWPART		(IO_STDCTRL|0x31)
#define	IO_ENDPART		(IO_STDCTRL|0x32)

#define	IOPP_COMMENT		(IO_STDCTRL|0x41)
#define	IOPP_EXPAND		(IO_STDCTRL|0x42)

#endif	/* EFEU/ioctrl.h */
