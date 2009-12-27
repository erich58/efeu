/*	Eingabekonvertierung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.6
*/

#ifndef	EFEU_InputEnc_h
#define	EFEU_InputEnc_h	1

#include <EFEU/io.h>
#include <EFEU/vecbuf.h>

#define	MSG_INPUTENC	"InputEnc"

typedef struct {
	uchar_t *name;
	uchar_t *value;
} InputEncEntry_t;

typedef struct {
	REFVAR;		/* Referenzvariablen */
	char *name;	/* Kodierungsname */
	vecbuf_t tab;	/* Kodierungstabelle */
} InputEnc_t;

extern reftype_t InputEnc_reftype;

InputEnc_t *InputEnc (const char *name);
void InputEnc_load (InputEnc_t *enc, io_t *io);
int InputEnc_print (io_t *io, InputEnc_t *enc);

io_t *io_InputEnc (io_t *base, InputEnc_t *enc);

#endif	/* EFEU_inputenc_h */
