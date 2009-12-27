/*
Makrowerkzeuge

$Header <EFEU/$1>

$Copyright (C) 1999 Erich Frühstück
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

#ifndef	_EFEU_mactools_h
#define	_EFEU_mactools_h	1

#if	__STDC__

#define	_String(x)	#x
#define	_Concat2(a,b)	a##b
#define	_Concat3(a,b,c)	a##b##c
#define	_Concat4(a,b,c,d)	a##b##c##d

#define	String(x)	_String(x)
#define	Concat2(a,b)	_Concat2(a,b)
#define	Concat3(a,b)	_Concat3(a,b,c)
#define	Concat4(a,b)	_Concat4(a,b,c,d)

#else

#define	String(x)	"x"
#define	Concat2(a,b)	a/**/b
#define	Concat3(a,b,c)	a/**/b/**/c
#define	Concat4(a,b,c,d)	a/**/b/**/c/**/d

#endif

#endif	/* EFEU/mactools.h */
