/*
Konvertierung EBCDIC - ASCII

$Copyright (C) 1995 Erich Fr�hst�ck
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

#include <EFEU/ebcdic.h>

static uchar_t ebcdic[] = {
	  0,	  1,	  2,	  3,	156,	  9,	134,	127,
	151,	141,	142,	 11,	 12,	 13,	 14,	 15,
	 16,	 17,	 18,	 19,	157,	133,	  8,	135,
	 24,	 25,	146,	143,	 28,	 29,	 30,	 31,
	128,	129,	130,	131,	132,	 10,	 23,	 27,
	136,	137,	138,	139,	140,	  5,	  6,	  7,
	144,	145,	 22,	147,	148,	149,	150,	  4,
	152,	153,	154,	155,	 20,	 21,	158,	 26,
	 32,	160,	161,	162,	163,	164,	165,	166,
	167,	168,	213,	 46,	 60,	 40,	 43,	124,
	 38,	169,	170,	171,	172,	173,	174,	175,
	176,	177,	 33,	 36,	 42,	 41,	 59,	126,
	 45,	 47,	178,	179,	180,	181,	182,	183,
	184,	185,	203,	 44,	 37,	 95,	 62,	 63,
	186,	187,	188,	189,	190,	191,	192,	193,
	194,	 96,	 58,	 35,	 64,	 39,	 61,	 34,
	195,	 97,	 98,	 99,	100,	101,	102,	103,
	104,	105,	196,	197,	198,	199,	200,	201,
	202,	106,	107,	108,	109,	110,	111,	112,
	113,	114,	 94,	204,	205,	206,	207,	208,
	209,	229,	115,	116,	117,	118,	119,	120,
	121,	122,	210,	211,	212,	 91,	214,	215,
	216,	217,	218,	219,	220,	221,	222,	223,
	224,	225,	226,	227,	228,	 93,	230,	231,
	123,	 65,	 66,	 67,	 68,	 69,	 70,	 71,
	 72,	 73,	232,	233,	234,	235,	236,	237,
	125,	 74,	 75,	 76,	 77,	 78,	 79,	 80,
	 81,	 82,	238,	239,	240,	241,	242,	243,
	 92,	159,	 83,	 84,	 85,	 86,	 87,	 88,
	 89,	 90,	244,	245,	246,	247,	248,	249,
	 48,	 49,	 50,	 51,	 52,	 53,	 54,	 55,
	 56,	 57,	250,	251,	252,	253,	254,	255,
};


int ebcdic2ascii(int c)
{
	return ebcdic[c & 0xFF];
}
