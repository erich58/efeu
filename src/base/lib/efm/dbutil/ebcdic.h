/*
EBCDIC - Zeichencodes

$Header	<EFEU/$1>

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

#ifndef	EFEU_ebcdic_h
#define	EFEU_ebcdic_h	1

#include <EFEU/config.h>

#define	SPACE		0x40	/* Leerzeichen */

#define	DIGIT_0		0xF0	/* Ziffer 0 */
#define	DIGIT_1		0xF1	/* Ziffer 1 */
#define	DIGIT_2		0xF2	/* Ziffer 2 */
#define	DIGIT_3		0xF3	/* Ziffer 3 */
#define	DIGIT_4		0xF4	/* Ziffer 4 */
#define	DIGIT_5		0xF5	/* Ziffer 5 */
#define	DIGIT_6		0xF6	/* Ziffer 6 */
#define	DIGIT_7		0xF7	/* Ziffer 7 */
#define	DIGIT_8		0xF8	/* Ziffer 8 */
#define	DIGIT_9		0xF9	/* Ziffer 9 */

#define	LETTER_A	0xC1	/* Buchstabe A */
#define	LETTER_B	0xC2	/* Buchstabe B */
#define	LETTER_C	0xC3	/* Buchstabe C */
#define	LETTER_D	0xC4	/* Buchstabe D */
#define	LETTER_E	0xC5	/* Buchstabe E */
#define	LETTER_F	0xC6	/* Buchstabe F */
#define	LETTER_G	0xC7	/* Buchstabe G */
#define	LETTER_H	0xC8	/* Buchstabe H */
#define	LETTER_I	0xC9	/* Buchstabe I */
#define	LETTER_J	0xD1	/* Buchstabe J */
#define	LETTER_K	0xD2	/* Buchstabe K */
#define	LETTER_L	0xD3	/* Buchstabe L */
#define	LETTER_M	0xD4	/* Buchstabe M */
#define	LETTER_N	0xD5	/* Buchstabe N */
#define	LETTER_O	0xD6	/* Buchstabe O */
#define	LETTER_P	0xD7	/* Buchstabe P */
#define	LETTER_Q	0xD8	/* Buchstabe Q */
#define	LETTER_R	0xD9	/* Buchstabe R */
#define	LETTER_S	0xE2	/* Buchstabe S */
#define	LETTER_T	0xE3	/* Buchstabe T */
#define	LETTER_U	0xE4	/* Buchstabe U */
#define	LETTER_V	0xE5	/* Buchstabe V */
#define	LETTER_W	0xE6	/* Buchstabe W */
#define	LETTER_X	0xE7	/* Buchstabe X */
#define	LETTER_Y	0xE8	/* Buchstabe Y */
#define	LETTER_Z	0xE9	/* Buchstabe Z */

#define	PUNCT_DP	0x4B	/* Dezimalpunkt */

extern int ebcdic2ascii (int c);
extern int ascii2ebcdic (int c);

#endif	/* EFEU/ebcdic.h */
