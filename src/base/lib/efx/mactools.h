/*	Makrowerkzeuge
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 1.0
*/

#ifndef	_EFEU_mactools_h
#define	_EFEU_mactools_h	1

#if	__STDC__

#define	_String(x)	#x
#define	_Concat2(a,b)	a##b
#define	_Concat3(a,b,c)	a##b##c
#define	_Concat4(a,b,c,d)a##b##c

#define	String(x)	_String(x)
#define	Concat2(a,b)	_Concat2(a,b)
#define	Concat3(a,b)	_Concat3(a,b,c)
#define	Concat4(a,b)	_Concat4(a,b,c,d)

#else

#define	String(x)	"x"
#define	Concat2(a,b)	a/**/b
#define	Concat3(a,b,c)	a/**/b/**/c
#define	Concat4(a,b,c,d)a/**/b/**/c/**/d

#endif

#endif	/* EFEU/mactools.h */
